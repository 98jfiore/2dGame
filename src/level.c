#include <stdlib.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_sprite.h"

#include "level.h"
#include "ent_environ.h"
#include "ent_env_las.h"
#include "ent_npc.h"

Level *level_new()
{
	Level *level;
	level = (Level *)malloc(sizeof(Level));
	if (level == NULL)
	{
		slog("Failed to allocate memory for game level");
		return NULL;
	}
	memset(level, 0, sizeof(Level));
	return level; 
}

Level *level_load(const char *filename)
{
	Level *level;
	SJson *json, *leveljs, *tileMap, *row;
	const char *string;
	int rows, columns;
	int i, j, count, tileIndex;
	int wallCode, pitCode, spikeCode, laserCode, laserBaseCode, tileType;
	SJson *objectsjs, *objjs;
	int objCount;
	int objx, objy;
	int objSpeed, objCycle, objRange;
	const char *objType, *objStartDir;

	Vector2D position;

	if (filename == NULL)
	{
		slog("Need filename to load level");
		return NULL;
	}

	json = sj_load(filename);
	if (json == NULL)
	{
		slog(filename);
		slog("File could not be loaded");
		return NULL;
	}

	level = level_new();
	if (level == NULL)
	{
		slog("Level could not be created");
		sj_free(json);
		return NULL;
	}

	leveljs = sj_object_get_value(json, "level");
	if (leveljs == NULL)
	{
		slog("Level json missing level object");
		level_free(level);
		sj_free(json);
		return NULL;
	}

	string = sj_get_string_value(sj_object_get_value(leveljs, "bgImage"));
	if (string)
	{
		level->bgImage = gf2d_sprite_load_image(string);
	}

	string = sj_get_string_value(sj_object_get_value(leveljs, "tileSet"));
	if (string)
	{
		sj_get_integer_value(sj_object_get_value(leveljs, "tileWidth"), &level->tileWidth);
		sj_get_integer_value(sj_object_get_value(leveljs, "tileHeight"), &level->tileHeight);
		sj_get_integer_value(sj_object_get_value(leveljs, "tileFramesPerLine"), &level->tileFramesPerLine);
		sj_get_integer_value(sj_object_get_value(leveljs, "wallCode"), &wallCode);
		sj_get_integer_value(sj_object_get_value(leveljs, "pitCode"), &pitCode);
		sj_get_integer_value(sj_object_get_value(leveljs, "spikeCode"), &spikeCode);
		sj_get_integer_value(sj_object_get_value(leveljs, "laserCode"), &laserCode);
		sj_get_integer_value(sj_object_get_value(leveljs, "laserBaseCode"), &laserBaseCode);
		sj_get_float_value(sj_object_get_value(leveljs, "scale"), &level->scaleAmount);

		level->tileSet = gf2d_sprite_load_all(
			(char *)string,
			level->tileWidth,
			level->tileHeight,
			level->tileFramesPerLine);

		level->scaleVec = vector2d(level->scaleAmount, level->scaleAmount);
		level->scalePos = vector2d(0, 0);
	}

	tileMap = sj_object_get_value(leveljs, "tileMap");
	if (tileMap == NULL)
	{
		slog("Couldn't load tile map");
		level_free(level);
		sj_free(json);
		return NULL;
	}
	
	rows = sj_array_get_count(tileMap);
	level->levelHeight = rows;

	row = sj_array_get_nth(tileMap, 0);
	columns = sj_array_get_count(row);
	level->levelWidth = columns;

	count = rows * columns;
	level->tileMap = (TileTypes*)gfc_allocate_array(sizeof(TileTypes), count);
	if (level->tileMap == NULL)
	{
		level_free(level);
		sj_free(json);
		return NULL;
	}

	//Load in plain tiles and basic environment entities
	tileIndex = 0;
	for (j = 0; j < rows; ++j)
	{
		row = sj_array_get_nth(tileMap, j);
		if (row == NULL) continue;
		
		if (columns != sj_array_get_count(row))
		{
			slog("Row %i column count mismatch", j);
			continue;
		}
		
		for (i = 0; i < columns; ++i)
		{
			sj_get_integer_value(sj_array_get_nth(row, i), &tileType);
			if (tileType == wallCode)
			{
				position = vector2d((tileIndex % level->levelWidth) * level->tileSet->frame_w * level->scaleAmount, (tileIndex / level->levelWidth) * level->tileSet->frame_h * level->scaleAmount);
				wall_spawn(position, (char *)string, wallCode - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount);
				level->tileMap[tileIndex++] = 0;
			}
			else if (tileType == pitCode)
			{
				position = vector2d((tileIndex % level->levelWidth) * level->tileSet->frame_w * level->scaleAmount, (tileIndex / level->levelWidth) * level->tileSet->frame_h * level->scaleAmount);
				pit_spawn(position, (char *)string, pitCode - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount);
				level->tileMap[tileIndex++] = 0;
			}
			else if (tileType == spikeCode)
			{
				position = vector2d((tileIndex % level->levelWidth) * level->tileSet->frame_w * level->scaleAmount, (tileIndex / level->levelWidth) * level->tileSet->frame_h * level->scaleAmount);
				spike_spawn(position, (char *)string, spikeCode - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount);
				level->tileMap[tileIndex++] = 0;
			}
			else
			{
				level->tileMap[tileIndex++] = tileType;
			}
		}
	}

	//Load in enemies
	objectsjs = sj_object_get_value(leveljs, "enemies");
	if (objectsjs != NULL)
	{
		objCount = sj_array_get_count(objectsjs);
		for (i = 0; i < objCount; i++)
		{
			objjs = sj_array_get_nth(objectsjs, i);
			if (objjs == NULL)
			{
				slog("ENEMY Not found");
				continue;
			}
			objType = sj_get_string_value(sj_object_get_value(objjs, "type"));
			sj_get_integer_value(sj_object_get_value(objjs, "x"), &objx);
			sj_get_integer_value(sj_object_get_value(objjs, "y"), &objy);
			objStartDir = sj_get_string_value(sj_object_get_value(objjs, "startDir"));
			
			//printf("%s %i,%i %s\n", enemyType, enemyx, enemyy, enemyStartDir);

			position = vector2d(objx * level->tileSet->frame_w * level->scaleAmount, objy * level->tileSet->frame_h * level->scaleAmount);

			if (strcmp(objType, "robot") == 0)
			{
				if (strcmp(objStartDir, "north") == 0)
				{
					robot_spawn(position, MOV_NORTH);
				}
				else if (strcmp(objStartDir, "south") == 0)
				{
					robot_spawn(position, MOV_SOUTH);
				}
				else if (strcmp(objStartDir, "east") == 0)
				{
					robot_spawn(position, MOV_EAST);
				}
				else if (strcmp(objStartDir, "west") == 0)
				{
					robot_spawn(position, MOV_WEST);
				}
			}
		}
	}

	//Load in obstacles
	objectsjs = sj_object_get_value(leveljs, "obstacles");
	if (objectsjs != NULL)
	{
		objCount = sj_array_get_count(objectsjs);
		for (i = 0; i < objCount; i++)
		{
			objjs = sj_array_get_nth(objectsjs, i);
			if (objjs == NULL)
			{
				slog("ENEMY Not found");
				continue;
			}
			objType = sj_get_string_value(sj_object_get_value(objjs, "type"));
			sj_get_integer_value(sj_object_get_value(objjs, "x"), &objx);
			sj_get_integer_value(sj_object_get_value(objjs, "y"), &objy);
			objStartDir = sj_get_string_value(sj_object_get_value(objjs, "startDir"));

			//printf("%s %i,%i %s\n", enemyType, enemyx, enemyy, enemyStartDir);

			position = vector2d(objx * level->tileSet->frame_w * level->scaleAmount, objy * level->tileSet->frame_h * level->scaleAmount);

			if (strcmp(objType, "moving wall") == 0)
			{
				sj_get_integer_value(sj_object_get_value(objjs, "speed"), &objSpeed);
				if (strcmp(objStartDir, "north") == 0)
				{
					moving_wall_spawn(position, (char *)string, wallCode - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount, objSpeed, MOV_NORTH);
				}
				else if (strcmp(objStartDir, "south") == 0)
				{
					moving_wall_spawn(position, (char *)string, wallCode - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount, objSpeed, MOV_SOUTH);
				}
				else if (strcmp(objStartDir, "east") == 0)
				{
					moving_wall_spawn(position, (char *)string, wallCode - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount, objSpeed, MOV_EAST);
				}
				else if (strcmp(objStartDir, "west") == 0)
				{
					moving_wall_spawn(position, (char *)string, wallCode - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount, objSpeed, MOV_WEST);
				}
			}

			else if (strcmp(objType, "laser") == 0)
			{
				sj_get_integer_value(sj_object_get_value(objjs, "cycle"), &objCycle);
				sj_get_integer_value(sj_object_get_value(objjs, "range"), &objRange);
				if (strcmp(objStartDir, "north") == 0)
				{
					laserBase_spawn(position, (char *)string, laserBaseCode - 1, laserCode - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount, objCycle, objRange, MOV_NORTH);
				}
				else if (strcmp(objStartDir, "south") == 0)
				{
					laserBase_spawn(position, (char *)string, laserBaseCode - 1, laserCode - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount, objCycle, objRange, MOV_SOUTH);
				}
				else if (strcmp(objStartDir, "east") == 0)
				{
					laserBase_spawn(position, (char *)string, laserBaseCode - 1, laserCode - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount, objCycle, objRange, MOV_EAST);
				}
				else if (strcmp(objStartDir, "west") == 0)
				{
					laserBase_spawn(position, (char *)string, laserBaseCode - 1, laserCode - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount, objCycle, objRange, MOV_WEST);
				}
			}
		}
	}

	sj_free(json);
	return level;
}

void level_free(Level *level)
{
	if (level == NULL)
	{
		return;
	}

	if (level->tileMap != NULL)
	{
		free(level->tileMap);
		level->tileMap = NULL;
	}

	gf2d_sprite_free(level->bgImage);

	gf2d_sprite_free(level->tileSet);

	free(level);
}

void level_draw(Level *level)
{
	int i;
	
	if (level == NULL)
	{
		slog("Cannot draw null level");
		return;
	}

	//Draw background
	if (level->bgImage != NULL)
	{
		gf2d_sprite_draw_image(level->bgImage, vector2d(0, 0));
	}

	//Draw all tiles
	if (level->tileMap == NULL)
	{
		slog("No tiles loaded for level");
		return;
	}
	for (i = 0; i < level->levelWidth * level->levelHeight; i++)
	{
		if (level->tileMap[i] == 0) continue;
		//slog("%i: x is %i, y is %i\n", i, i % level->levelWidth, i / level->levelHeight);
		gf2d_sprite_draw(
			level->tileSet,
			vector2d((i % level->levelWidth) * level->tileSet->frame_w * level->scaleAmount, (i / level->levelWidth) * level->tileSet->frame_h * level->scaleAmount),
			&level->scaleVec,
			&level->scalePos,
			NULL,
			NULL,
			NULL,
			level->tileMap[i] - 1);
	}
}

/*eol@eof*/
