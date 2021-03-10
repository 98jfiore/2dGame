#include <stdlib.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_sprite.h"

#include "level.h"
#include "player.h"
#include "ui.h"
#include "ent_environ.h"
#include "ent_env_las.h"
#include "ent_npc.h"
#include "ent_npc_robot.h"
#include "ent_npc_drone.h"
#include "ent_npc_sweeper.h"
#include "ent_npc_automata.h"
#include "ent_npc_orb.h"
#include "ent_item.h"
#include "ent_upgrade.h"

static Level *thisLevel = {NULL};

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
	SJson *json, *leveljs;
	Entity *player;
	const char *string;
	char *saveFile;
	SJson *objjs;
	int objx, objy;

	Vector2D position;

	level = level_jsonload(filename);
	saveFile = "saves/save.json";

	json = sj_load(filename);
	if (json == NULL)
	{
		slog(filename);
		slog("File could not be loaded");
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

	//Load in player
	objjs = sj_object_get_value(leveljs, "player spawn");
	if (objjs != NULL)
	{
		sj_get_integer_value(sj_object_get_value(objjs, "x"), &objx);
		sj_get_integer_value(sj_object_get_value(objjs, "y"), &objy);

		position = vector2d(objx * level->tileSet->frame_w * level->scaleAmount, objy * level->tileSet->frame_h * level->scaleAmount);

		player = player_spawn(position);
		load_player(player, saveFile);
	}
	else
	{
		slog("Player spawn not found");
		level_free(level);
		entity_manager_free();
		sj_free(json);
		return NULL;
	}

	string = sj_get_string_value(sj_object_get_value(leveljs, "uiFormat"));
	ui_format_load(string, player);

	thisLevel = level;
	return level;
}

Level *level_loadWithPlayer(const char *filename, Entity *player, char *nextPlayerPos)
{
	Level *level;
	SJson *json, *leveljs;
	const char *string;
	char *saveFile;
	SJson *objjs, *positionjs;
	int objx, objy;

	level = level_jsonload(filename);
	saveFile = "saves/save.json";

	json = sj_load(filename);
	if (json == NULL)
	{
		slog(filename);
		slog("File could not be loaded");
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

	//Load in player
	objjs = sj_object_get_value(leveljs, "player spawn");
	if (objjs != NULL)
	{
		positionjs = sj_object_get_value(objjs, nextPlayerPos);
		if (positionjs != NULL)
		{
			sj_get_integer_value(sj_object_get_value(positionjs, "x"), &objx);
			sj_get_integer_value(sj_object_get_value(positionjs, "y"), &objy);
		}
		else
		{
			sj_get_integer_value(sj_object_get_value(objjs, "x"), &objx);
			sj_get_integer_value(sj_object_get_value(objjs, "y"), &objy);
		}
		player->position = vector2d(objx * level->tileSet->frame_w * level->scaleAmount, objy * level->tileSet->frame_h * level->scaleAmount);
		player->hitbox->x = player->position.x;
		player->hitbox->y = player->position.y;
	}
	else
	{
		slog("Player spawn not found");
		level_free(level);
		entity_manager_free();
		sj_free(json);
		return NULL;
	}

	string = sj_get_string_value(sj_object_get_value(leveljs, "uiFormat"));
	ui_format_load(string, player);

	thisLevel = level;
	return level;
}


Level *level_jsonload(const char *filename)
{
	Level *level;
	SJson *json, *leveljs, *tileMap, *row;
	//Entity *player;
	const char *string;
	SJson *savedjs, *savedObjjs;
	int saveFound;
	char *saveFile;
	int rows, columns;
	int i, j, count, tileIndex;
	int wallCode, pitCode, spikeCode, laserCode, laserBaseCode, goldDoorCode, tileType;
	SJson *objectsjs, *objjs;
	int objCount;
	int objx, objy;
	int objSpeed, objCycle, objRange;
	const char *objType, *objStartDir, *objTag;
	const char *transPoint, *transLevel;
	int spriteFrame;

	Vector2D position;

	//Entity *test;
	//Upgrade *upTest;

	saveFile = "saves/save.json";
	savedjs = sj_load(saveFile);

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
		sj_get_integer_value(sj_object_get_value(leveljs, "goldDoorCode"), &goldDoorCode);
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
			else if (strcmp(objType, "drone") == 0)
			{
				if (strcmp(objStartDir, "north") == 0)
				{
					drone_spawn(position, MOV_NORTH);
				}
				else if (strcmp(objStartDir, "south") == 0)
				{
					drone_spawn(position, MOV_SOUTH);
				}
				else if (strcmp(objStartDir, "east") == 0)
				{
					drone_spawn(position, MOV_EAST);
				}
				else if (strcmp(objStartDir, "west") == 0)
				{
					drone_spawn(position, MOV_WEST);
				}
			}
			else if (strcmp(objType, "sweeper") == 0)
			{
				sj_get_integer_value(sj_object_get_value(objjs, "cycle"), &objCycle);
				if (strcmp(objStartDir, "north") == 0)
				{
					sweeper_spawn(position, MOV_NORTH, objCycle);
				}
				else if (strcmp(objStartDir, "south") == 0)
				{
					sweeper_spawn(position, MOV_SOUTH, objCycle);
				}
				else if (strcmp(objStartDir, "east") == 0)
				{
					sweeper_spawn(position, MOV_EAST, objCycle);
				}
				else if (strcmp(objStartDir, "west") == 0)
				{
					sweeper_spawn(position, MOV_WEST, objCycle);
				}
			}
			else if (strcmp(objType, "automata") == 0)
			{
				sj_get_integer_value(sj_object_get_value(objjs, "cycle"), &objCycle);
				if (strcmp(objStartDir, "north") == 0)
				{
					automata_spawn(position, MOV_NORTH, objCycle);
				}
				else if (strcmp(objStartDir, "south") == 0)
				{
					automata_spawn(position, MOV_SOUTH, objCycle);
				}
				else if (strcmp(objStartDir, "east") == 0)
				{
					automata_spawn(position, MOV_EAST, objCycle);
				}
				else if (strcmp(objStartDir, "west") == 0)
				{
					automata_spawn(position, MOV_WEST, objCycle);
				}
			}
			else if (strcmp(objType, "orb") == 0)
			{
				sj_get_integer_value(sj_object_get_value(objjs, "cycle"), &objCycle);
				if (strcmp(objStartDir, "north") == 0)
				{
					orb_spawn(position, MOV_NORTH, objCycle);
				}
				else if (strcmp(objStartDir, "south") == 0)
				{
					orb_spawn(position, MOV_SOUTH, objCycle);
				}
				else if (strcmp(objStartDir, "east") == 0)
				{
					orb_spawn(position, MOV_EAST, objCycle);
				}
				else if (strcmp(objStartDir, "west") == 0)
				{
					orb_spawn(position, MOV_WEST, objCycle);
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

			else if (strcmp(objType, "goldDoor") == 0)
			{
				goldDoor_spawn(position, (char *)string, goldDoorCode - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount);
			}



			else if (strcmp(objType, "transition") == 0)
			{
				transPoint = sj_get_string_value(sj_object_get_value(objjs, "toPoint"));
				transLevel = sj_get_string_value(sj_object_get_value(objjs, "toLevel"));
				sj_get_integer_value(sj_object_get_value(objjs, "spriteFrame"), &spriteFrame);
				transition_spawn(position, (char *)string, spriteFrame - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount, transLevel, transPoint, level);
			}
		}
	}

	//Load in items
	objectsjs = sj_object_get_value(leveljs, "items");
	if (objectsjs != NULL)
	{
		objCount = sj_array_get_count(objectsjs);
		for (i = 0; i < objCount; i++)
		{
			objjs = sj_array_get_nth(objectsjs, i);
			if (objjs == NULL)
			{
				slog("ITEM Not found");
				continue;
			}
			objType = sj_get_string_value(sj_object_get_value(objjs, "type"));
			sj_get_integer_value(sj_object_get_value(objjs, "x"), &objx);
			sj_get_integer_value(sj_object_get_value(objjs, "y"), &objy);

			position = vector2d(objx * level->tileSet->frame_w * level->scaleAmount, objy * level->tileSet->frame_h * level->scaleAmount);

			if (strcmp(objType, "healing1") == 0)
			{
				healing1_spawn(position);
			}
			else if (strcmp(objType, "healing2") == 0)
			{
				healing2_spawn(position);
			}
			else if (strcmp(objType, "healing3") == 0)
			{
				healing3_spawn(position);
			}
			else if (strcmp(objType, "invin1") == 0)
			{
				invin1_spawn(position);
			}
			else if (strcmp(objType, "invin2") == 0)
			{
				invin2_spawn(position);
			}
		}
	}

	//Load in upgrade items
	objectsjs = sj_object_get_value(leveljs, "upgrades");
	if (objectsjs != NULL)
	{
		objCount = sj_array_get_count(objectsjs);
		for (i = 0; i < objCount; i++)
		{
			objjs = sj_array_get_nth(objectsjs, i);
			if (objjs == NULL)
			{
				slog("UPGRADE Not found");
				continue;
			}
			objType = sj_get_string_value(sj_object_get_value(objjs, "type"));
			sj_get_integer_value(sj_object_get_value(objjs, "x"), &objx);
			sj_get_integer_value(sj_object_get_value(objjs, "y"), &objy);
			objTag = sj_get_string_value(sj_object_get_value(objjs, "tag"));

			position = vector2d(objx * level->tileSet->frame_w * level->scaleAmount, objy * level->tileSet->frame_h * level->scaleAmount);

			if (savedjs == NULL)
			{
				if (strcmp(objType, "health") == 0)
				{
					health_spawn(position, objTag);
				}
				else if (strcmp(objType, "beamSword") == 0)
				{
					swordUpgrade_spawn(position, objTag);
				}
				else if (strcmp(objType, "sword") == 0)
				{
					sword_spawn(position, objTag);
				}
				else if (strcmp(objType, "key") == 0)
				{
					key_spawn(position, objTag);
				}
				else if (strcmp(objType, "bombUpgrade") == 0)
				{
					bombUpgrade_spawn(position, objTag);
				}
			}
			else
			{
				savedObjjs = sj_object_get_value(savedjs, objTag);
				if (savedObjjs == NULL)
				{
					if (strcmp(objType, "health") == 0)
					{
						health_spawn(position, objTag);
					}
					else if (strcmp(objType, "beamSword") == 0)
					{
						swordUpgrade_spawn(position, objTag);
					}
					else if (strcmp(objType, "sword") == 0)
					{
						sword_spawn(position, objTag);
					}
					else if (strcmp(objType, "key") == 0)
					{
						key_spawn(position, objTag);
					}
					else if (strcmp(objType, "bombUpgrade") == 0)
					{
						bombUpgrade_spawn(position, objTag);
					}
				}
				else
				{
					sj_get_integer_value(savedObjjs, &saveFound);
					if (saveFound != 1)
					{
						if (strcmp(objType, "health") == 0)
						{
							health_spawn(position, objTag);
						}
						else if (strcmp(objType, "beamSword") == 0)
						{
							swordUpgrade_spawn(position, objTag);
						}
						else if (strcmp(objType, "sword") == 0)
						{
							sword_spawn(position, objTag);
						}
						else if (strcmp(objType, "key") == 0)
						{
							key_spawn(position, objTag);
						}
						else if (strcmp(objType, "bombUpgrade") == 0)
						{
							bombUpgrade_spawn(position, objTag);
						}
					}
				}
			}
		}
	}


	sj_free(json);
	return level;
}

void level_transition(const char *nextLevel, char *nextPos, Level *currentLevel)
{
	Entity *player;

	player = level_unload(currentLevel);
	//entity_free(player);
	level_loadWithPlayer(nextLevel, player, nextPos);
}

Entity *level_unload(Level *level)
{
	Entity *player;

	if (level == thisLevel)
	{
		thisLevel = NULL;
	}

	player = delete_notplayer();
	ui_manager_free();

	if (level != NULL)
	{
		level_free(level);
	}

	return player;
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

	//gf2d_sprite_free(level->bgImage);

	//gf2d_sprite_free(level->tileSet);

	free(level);
}

void level_draw()
{
	int i;
	
	if (thisLevel == NULL)
	{
		slog("Cannot draw null level");
		return;
	}

	//Draw background
	if (thisLevel->bgImage != NULL)
	{
		gf2d_sprite_draw_image(thisLevel->bgImage, vector2d(0, 0));
	}

	//Draw all tiles
	if (thisLevel->tileMap == NULL)
	{
		slog("No tiles loaded for level");
		return;
	}
	for (i = 0; i < thisLevel->levelWidth * thisLevel->levelHeight; i++)
	{
		if (thisLevel->tileMap[i] == 0) continue;
		//slog("%i: x is %i, y is %i\n", i, i % level->levelWidth, i / level->levelHeight);
		gf2d_sprite_draw(
			thisLevel->tileSet,
			vector2d((i % thisLevel->levelWidth) * thisLevel->tileSet->frame_w * thisLevel->scaleAmount, (i / thisLevel->levelWidth) * thisLevel->tileSet->frame_h * thisLevel->scaleAmount),
			&thisLevel->scaleVec,
			&thisLevel->scalePos,
			NULL,
			NULL,
			NULL,
			thisLevel->tileMap[i] - 1);
	}
}

Entity *transition_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale, char *nextLevel, char *nextPos, Level *level)
{
	Entity *ent;
	Rect *hitbox;
	Transition *trans;

	ent = environment_spawn(position, spriteSheet, frameNum, spriteWidth, spriteHeight, fpl, scale);
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	hitbox = (Rect *)malloc(sizeof(Rect));
	if (hitbox == NULL)
	{
		slog("Could not create hitbox");
		free(ent);
		return NULL;
	}

	hitbox->x = position.x + 5;
	hitbox->y = position.y + 5;
	hitbox->width = spriteWidth * scale - 10;
	hitbox->height = spriteHeight * scale - 10;
	ent->hitbox = hitbox;

	ent->flags = ENT_TRANSITION;

	trans = (Transition *)malloc(sizeof(Transition));
	if (trans == NULL)
	{
		slog("Could not allocate space for transition");
		entity_free(ent);
		return NULL;
	}
	trans->nextLevel = malloc(strlen(nextLevel) + 1);
	strcpy(trans->nextLevel, nextLevel);
	trans->nextPos = malloc(strlen(nextPos) + 1);
	strcpy(trans->nextPos, nextPos);
	trans->currentLevel = level;
	ent->data = trans;

	ent->free = transition_free;
	ent->update = transition_update;

	return ent;
}

void transition_update(Entity *self)
{
	Entity *collided;
	Transition *trans;

	if (self == NULL)
	{
		slog("Can't update a null entity");
		return;
	}

	if (self->hitbox != NULL)
	{
		collided = check_collision(self);
		if (collided)
		{
			if (collided->flags & ENT_PLAYER)
			{
				trans = (Transition *)self->data;
				if (trans == NULL)
				{
					slog("Can't transition with no info");
					return;
				}
				level_transition(trans->nextLevel, trans->nextPos, trans->currentLevel);
			}
		}
	}
}

void transition_free(Entity *self)
{
	Transition *trans;

	if (self == NULL)
	{
		slog("Can't free a null entity");
		return;
	}

	trans = (Transition *)self->data;
	if (trans == NULL)
	{
		slog("Can't free nonexistent door");
		return;
	}

	free(trans->nextLevel);
	free(trans->nextPos);
	free(trans);
}

/*eol@eof*/
