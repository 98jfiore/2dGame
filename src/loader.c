#include <stdlib.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_sprite.h"
#include "gfc_audio.h"

#include "loader.h"
#include "level.h"
#include "player.h"
#include "ui.h"
#include "menu.h"
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
#include "bosses.h"
#include "event.h"

static Level *thisLevel = { NULL };
static char *playerFile = "saves/save.json";
static char *saveFile = "";
static int levelEditCode = 0;

Level *level_load(const char *filename)
{
	Level *level;
	SJson *json, *leveljs;
	Entity *player;
	const char *string;
	//char *playerFile;
	SJson *objjs;
	int objx, objy;

	Vector2D position;

	level = level_jsonload(filename);
	//playerFile = "saves/save.json";

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
	json = sj_object_get_value(leveljs, "isPlayer");
	if (json == NULL || strcmp(sj_get_string_value(json), "False") != 0)
	{
		objjs = sj_object_get_value(leveljs, "player spawn");
		if (objjs != NULL)
		{
			sj_get_integer_value(sj_object_get_value(objjs, "x"), &objx);
			sj_get_integer_value(sj_object_get_value(objjs, "y"), &objy);

			position = vector2d(objx * level->tileSet->frame_w * level->scaleAmount, objy * level->tileSet->frame_h * level->scaleAmount);

			player = player_spawn(position);
			load_player(player, playerFile);

			string = sj_get_string_value(sj_object_get_value(leveljs, "uiFormat"));
			ui_format_load(string, player);
		}
	}

	thisLevel = level;
	return level;
}

Level *level_loadWithPlayer(const char *filename, Entity *player, char *nextPlayerPos)
{
	Level *level;
	SJson *json, *leveljs;
	const char *string;
	SJson *objjs, *positionjs;
	int objx, objy;

	level = level_jsonload(filename);

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
	int rows, columns;
	int i, j, count, tileIndex;
	int wallCode, pitCode, spikeCode, laserCode, laserBaseCode, goldDoorCode, tileType;
	SJson *objectsjs, *objjs;
	Sound *music;
	int objCount;
	int objx, objy, objwidth, objheight;
	int objSpeed, objCycle, objRange;
	const char *objType, *objStartDir, *objTag;
	const char *transPoint, *transLevel;
	int spriteFrame;
	char *menuFile;

	Vector2D position;

	//Entity *test;
	//Upgrade *upTest;

	savedjs = sj_load(playerFile);

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

	//Load music
	string = sj_get_string_value(sj_object_get_value(leveljs, "music"));
	if (string)
	{
		music = gfc_sound_load(string, 1, -1);
		if (music == NULL)
		{
			slog("Music file could not be loaded");
		}
		else
		{
			gfc_set_background_music(music);

		}
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
		slog("No tile map");
	}
	else
	{

		rows = sj_array_get_count(tileMap);
		level->levelHeight = rows;

		row = sj_array_get_nth(tileMap, 0);
		columns = sj_array_get_count(row);
		level->levelWidth = columns;

		count = rows * columns;
		level->tileMap = (int*)gfc_allocate_array(sizeof(int), count);
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

		//Load in bosses
		objectsjs = sj_object_get_value(leveljs, "bosses");
		if (objectsjs != NULL)
		{
			objCount = sj_array_get_count(objectsjs);
			for (i = 0; i < objCount; i++)
			{
				objjs = sj_array_get_nth(objectsjs, i);
				if (objjs == NULL)
				{
					slog("BOSS Not found");
					continue;
				}
				objType = sj_get_string_value(sj_object_get_value(objjs, "type"));
				sj_get_integer_value(sj_object_get_value(objjs, "x"), &objx);
				sj_get_integer_value(sj_object_get_value(objjs, "y"), &objy);

				//printf("%s %i,%i %s\n", enemyType, enemyx, enemyy, enemyStartDir);

				position = vector2d(objx * level->tileSet->frame_w * level->scaleAmount, objy * level->tileSet->frame_h * level->scaleAmount);

				if (strcmp(objType, "boss1") == 0)
				{
					boss1_spawn(position);
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

				else if (strcmp(objType, "savePoint") == 0)
				{
					sj_get_integer_value(sj_object_get_value(objjs, "spriteFrame"), &spriteFrame);
					savePoint_spawn(position, (char *)string, spriteFrame - 1, level->tileWidth, level->tileHeight, level->tileFramesPerLine, level->scaleAmount, filename);
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


		//Load in events
		objectsjs = sj_object_get_value(leveljs, "events");
		if (objectsjs != NULL)
		{
			objCount = sj_array_get_count(objectsjs);
			for (i = 0; i < objCount; i++)
			{
				objjs = sj_array_get_nth(objectsjs, i);
				if (objjs == NULL)
				{
					slog("EVENT Not found");
					continue;
				}
				objType = sj_get_string_value(sj_object_get_value(objjs, "event"));
				sj_get_integer_value(sj_object_get_value(objjs, "x"), &objx);
				sj_get_integer_value(sj_object_get_value(objjs, "y"), &objy);
				sj_get_integer_value(sj_object_get_value(objjs, "width"), &objwidth);
				sj_get_integer_value(sj_object_get_value(objjs, "height"), &objheight);
				position = vector2d(objx * level->tileSet->frame_w * level->scaleAmount, objy * level->tileSet->frame_h * level->scaleAmount);
				if (savedjs == NULL)
				{
					event_trigger_spawn(position, objwidth * level->tileSet->frame_w * level->scaleAmount, objheight * level->tileSet->frame_h * level->scaleAmount, objType);
				}
				else
				{
					savedObjjs = sj_object_get_value(savedjs, objType);
					if (savedObjjs == NULL)
					{
						event_trigger_spawn(position, objwidth * level->tileSet->frame_w * level->scaleAmount, objheight * level->tileSet->frame_h * level->scaleAmount, objType);
					}
					else
					{
						sj_get_integer_value(savedObjjs, &saveFound);
						if (saveFound != 1)
						{
							event_trigger_spawn(position, objwidth * level->tileSet->frame_w * level->scaleAmount, objheight * level->tileSet->frame_h * level->scaleAmount, objType);
						}
					}
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
	}

	menuFile = sj_get_string_value(sj_object_get_value(leveljs, "menu"));
	if (menuFile != NULL)
	{
		menu_format_load(menuFile);
	}

	sj_free(json);
	return level;
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
	menu_manager_free();

	if (level != NULL)
	{
		level_free(level);
	}

	return player;
}

void level_unload_no_player(Level *level)
{
	if (level == thisLevel)
	{
		thisLevel = NULL;
	}

	delete_all_ent();
	ui_manager_free();
	menu_manager_free();

	if (level != NULL)
	{
		level_free(level);
	}

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
		//slog("No tiles loaded for level");
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

void level_transition(const char *nextLevel)
{
	level_unload(thisLevel);
	level_load(nextLevel);
}

void level_transition_no_player(const char *nextLevel)
{
	saveFile = "";
	level_unload_no_player(thisLevel);
	level_load(nextLevel);
}

void level_transitionNewGame(const char *nextLevel)
{

	SJson *baseJson;

	baseJson = sj_object_new();
	sj_save(baseJson, playerFile);

	level_unload(thisLevel);
	level_load(nextLevel);
}

void menu_level_transition(MenuComponent *self)
{
	level_transition_no_player(self->action_specification);
	resume_game();
}

void menu_level_transitionNewGame(MenuComponent *self)
{
	change_save_file(self->action_specification);
	level_transitionNewGame("defs/room1_1.json");
	resume_game();
}

void level_transitionWithPlayer(const char *nextLevel, char *nextPos, Level *currentLevel)
{
	Entity *player;

	player = level_unload(currentLevel);
	//entity_free(player);
	level_loadWithPlayer(nextLevel, player, nextPos);
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
				level_transitionWithPlayer(trans->nextLevel, trans->nextPos, trans->currentLevel);
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

Entity *savePoint_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale, char *currentLevel)
{
	Entity *ent;
	Rect *hitbox;
	SavePoint *sp;

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

	ent->flags = 0;

	sp = (SavePoint *)malloc(sizeof(SavePoint));
	if (sp == NULL)
	{
		slog("Could not allocate space for savePoint");
		entity_free(ent);
		return NULL;
	}
	sp->active = SDL_TRUE;
	sp->currentLevel = malloc((strlen(currentLevel) + 1) * sizeof(char));
	strcpy(sp->currentLevel, currentLevel);
	slog(currentLevel);
	ent->data = sp;

	ent->free = savePoint_free;
	ent->update = savePoint_update;

	return ent;
}

void savePoint_update(Entity *self)
{
	SDL_bool collided;
	SavePoint *sp;
	int clickX, clickY;

	if (self == NULL)
	{
		slog("Can't update a null entity");
		return;
	}

	sp = (SavePoint *)self->data;
	if (sp == NULL || sp->active == SDL_FALSE)
	{
		return;
	}

	if (self->hitbox != NULL)
	{
		if (SDL_GetMouseState(&clickX, &clickY) & SDL_BUTTON(SDL_BUTTON_LEFT))
		{
			collided = check_collision_with_player(self);
			if (collided == SDL_TRUE)
			{
				save_player_from_savePoint(self);
				sp->active = SDL_FALSE;
				self->baseFrame++;
				self->frame++;
			}
		}
	}
}

void savePoint_free(Entity *self)
{
	SavePoint *sp;

	if (self == NULL)
	{
		slog("Can't free a null entity");
		return;
	}

	sp = (SavePoint *)self->data;
	if (sp == NULL)
	{
		slog("Can't free nonexistent point");
		return;
	}
	free(sp->currentLevel);
	free(sp);
}

void save_player_from_savePoint(Entity *self)
{
	SJson *playerJson, *saveJson;
	SavePoint *sp;

	if (self == NULL)
	{
		slog("Can't update a null entity");
		return;
	}

	sp = (SavePoint *)self->data;
	if (sp == NULL || sp->active == SDL_FALSE)
	{
		return;
	}

	//Save the player
	save_player(get_player(), playerFile);

	playerJson = sj_load(playerFile);
	if (playerJson == NULL)
	{
		playerJson = sj_object_new();
	}
	if (saveFile == NULL || strlen(saveFile) == 0)
	{
		slog("No place to save game");
		return;
	}

	saveJson = sj_object_new();
	sj_object_insert(saveJson, "player", playerJson);
	sj_object_insert(saveJson, "location", sj_new_str(sp->currentLevel));
	sj_save(saveJson, saveFile);
}

void change_save_file(const char *filename)
{
	saveFile = malloc((strlen(filename) + 1) * sizeof(char));
	strcpy(saveFile, filename);
}

void menu_format_load(const char *filename)
{
	SJson *json, *menujs, *compsjs, *compjs;
	int menuWidth, menuHeight, compCount;
	SDL_bool check;
	MenuComponent *comp;

	SJson *colorjs;
	Color text_color;
	char *text, *fontFile, *spriteFile, *action, *specification;
	int text_ptsize, color_r, color_g, color_b, color_a;
	int sprite_w, sprite_h, frames_per_line, frame_count, frame_num, scale;
	int menux, menuy, offx, offy, x, y;

	int i;

	if (filename == NULL)
	{
		slog("Need filename to load ui setup");
		return;
	}

	json = sj_load(filename);
	if (json == NULL)
	{
		slog(filename);
		slog("File could not be loaded");
		return;
	}

	menujs = sj_object_get_value(json, "menu");
	if (menujs == NULL)
	{
		slog("Menu JSon missing JSon object");
		sj_free(json);
		return;
	}

	sj_get_integer_value(sj_object_get_value(menujs, "width"), &menuWidth);
	sj_get_integer_value(sj_object_get_value(menujs, "height"), &menuHeight);

	if (menuHeight == 0 || menuWidth == 0)
	{
		slog("Cannot create empty menu");
		sj_free(json);
		return;
	}

	check = menu_manager_init(menuWidth, menuHeight);
	if (check == SDL_FALSE)
	{
		slog("Menu system could not be created");
		sj_free(json);
		return;
	}

	compsjs = sj_object_get_value(menujs, "components");
	if (compsjs != NULL)
	{
		compCount = sj_array_get_count(compsjs);
		for (i = 0; i < compCount; ++i)
		{
			compjs = sj_array_get_nth(compsjs, i);
			if (compjs == NULL)
			{
				slog("Component load error");
				return;
			}

			spriteFile = sj_get_string_value(sj_object_get_value(compjs, "sprite"));

			sj_get_integer_value(sj_object_get_value(compjs, "sprWidth"), &sprite_w);
			sj_get_integer_value(sj_object_get_value(compjs, "sprHeight"), &sprite_h);
			sj_get_integer_value(sj_object_get_value(compjs, "frames_per_line"), &frames_per_line);
			sj_get_integer_value(sj_object_get_value(compjs, "frame_count"), &frame_count);
			sj_get_integer_value(sj_object_get_value(compjs, "frame_num"), &frame_num);
			sj_get_integer_value(sj_object_get_value(compjs, "scale"), &scale);

			sj_get_integer_value(sj_object_get_value(compjs, "menux"), &menux);
			sj_get_integer_value(sj_object_get_value(compjs, "menuy"), &menuy);
			sj_get_integer_value(sj_object_get_value(compjs, "x"), &x);
			sj_get_integer_value(sj_object_get_value(compjs, "y"), &y);

			action = sj_get_string_value(sj_object_get_value(compjs, "action"));
			specification = sj_get_string_value(sj_object_get_value(compjs, "specification"));

			text = sj_get_string_value(sj_object_get_value(compjs, "text"));
			if (text == NULL || strlen(text) == 0)
			{
				comp = menu_component_create_no_text(spriteFile, sprite_w, sprite_h, frames_per_line, frame_count, frame_num, scale, menux, menuy, x, y, action, specification);

			}
			else
			{
				fontFile = sj_get_string_value(sj_object_get_value(compjs, "font"));
				sj_get_integer_value(sj_object_get_value(compjs, "pt_size"), &text_ptsize);

				colorjs = sj_object_get_value(compjs, "color");
				if (colorjs != NULL)
				{
					sj_get_integer_value(sj_object_get_value(colorjs, "r"), &color_r);
					sj_get_integer_value(sj_object_get_value(colorjs, "g"), &color_g);
					sj_get_integer_value(sj_object_get_value(colorjs, "b"), &color_b);
					sj_get_integer_value(sj_object_get_value(colorjs, "a"), &color_a);
				}
				else
				{
					color_r = 0;
					color_g = 0;
					color_b = 0;
					color_a = 255;
				}
				sj_get_integer_value(sj_object_get_value(compjs, "offsetx"), &offx);
				sj_get_integer_value(sj_object_get_value(compjs, "offsety"), &offy);

				text_color = gfc_color8(color_r, color_g, color_b, color_a);

				comp = menu_component_create(text, fontFile, text_ptsize, text_color, spriteFile, sprite_w, sprite_h, frames_per_line, frame_count, frame_num, scale, menux, menuy, offx, offy, x, y, action, specification);
			}

			spriteFile = NULL;
			spriteFile = sj_get_string_value(sj_object_get_value(compjs, "second_sprite"));
			if (spriteFile != NULL && strlen(spriteFile) != 0)
			{
				sj_get_integer_value(sj_object_get_value(compjs, "second_sprWidth"), &sprite_w);
				sj_get_integer_value(sj_object_get_value(compjs, "second_sprHeight"), &sprite_h);
				sj_get_integer_value(sj_object_get_value(compjs, "second_frames_per_line"), &frames_per_line);
				sj_get_integer_value(sj_object_get_value(compjs, "second_frame_count"), &frame_count);
				sj_get_integer_value(sj_object_get_value(compjs, "second_frame_num"), &frame_num);
				sj_get_integer_value(sj_object_get_value(compjs, "second_scale"), &scale);
				sj_get_integer_value(sj_object_get_value(compjs, "offsetx"), &offx);
				sj_get_integer_value(sj_object_get_value(compjs, "offsety"), &offy);
				comp->second_sprite = malloc(sizeof(SecondSprite));
				comp->second_sprite->second_sprite = gf2d_sprite_load_all(spriteFile, sprite_w, sprite_h, frames_per_line);
				comp->second_sprite->second_baseFrame = frame_num;
				comp->second_sprite->second_frame = frame_num;
				comp->second_sprite->second_frameRate = 0;
				comp->second_sprite->second_frameCount = frame_count;
				comp->second_sprite->second_scale = vector2d(scale, scale);
				comp->second_sprite->second_position = vector2d(x + offx, y + offy);
				if (comp->second_sprite->second_sprite == NULL)
				{
					slog("Menu sprite couldn't load");
					free(comp);
				}
			}

			if (i == 0 && comp != NULL)
			{
				comp->flags = MENU_SELECTED;
				menu_manager_set_pos(menux, menuy);
			}
		}
	}
	sj_free(json);
}


MenuComponent *menu_component_create(char *text, char *fontFile, Uint32 text_ptsize, Color text_color, const char *spriteFile, int sprite_w, int sprite_h, int sprite_fpl, int sprite_count, int sprite_num, int sprite_scale, int menu_x, int menu_y, int offset_x, int offset_y, int x, int y, char *action, char *specification)
{
	MenuComponent *comp = menu_component_create_no_text(spriteFile, sprite_w, sprite_h, sprite_fpl, sprite_count, sprite_num, sprite_scale, menu_x, menu_y, x, y, action, specification);

	comp->text = malloc(sizeof(MenuText));

	comp->text->font = font_load(fontFile, text_ptsize);
	comp->text->text_color = text_color;
	comp->text->text = malloc((strlen(text) + 1) * sizeof(char));
	strcpy(comp->text->text, text);

	comp->text->textPosition = vector2d(x + offset_x, y + offset_y);

	return comp;
}

MenuComponent *menu_component_create_no_text(const char *spriteFile, int sprite_w, int sprite_h, int sprite_fpl, int sprite_count, int sprite_num, int sprite_scale, int menu_x, int menu_y, int x, int y, char *action, char *specification)
{
	MenuComponent *comp;
	comp = menu_component_new(menu_x, menu_y);
	if (comp == NULL)
	{
		slog("Failed to create component");
		return NULL;
	}

	comp->position = vector2d(x, y);
	comp->sprite = gf2d_sprite_load_all(spriteFile, sprite_w, sprite_h, sprite_fpl);
	if (comp->sprite == NULL)
	{
		slog("UI sprite couldn't load");
		free(comp);
		return NULL;
	}
	comp->frameCount = sprite_count;
	comp->baseFrame = sprite_num;
	comp->scale = vector2d(sprite_scale, sprite_scale);
	comp->frameRate = 0;
	comp->color = NULL;

	comp->draw = NULL;
	comp->update = NULL;
	comp->free = NULL;

	comp->text = NULL;

	comp->action_specification = malloc((strlen(specification) + 1) * sizeof(char));
	strcpy(comp->action_specification, specification);

	//printf("%s\n%s\n", comp->text, comp->action_specification);
	if (strcmp(action, "load_game") == 0)
	{
		comp->action = menu_level_transition;
	}
	else if (strcmp(action, "new_game") == 0)
	{
		comp->action = menu_level_transitionNewGame;
	}
	else if (strcmp(action, "add_to_level") == 0)
	{
		comp->action = menu_add_to_level;
	}
	else if (strcmp(action, "change_code") == 0)
	{
		comp->action = menu_change_edit_code;
	}
	else if (strcmp(action, "save_level") == 0)
	{
		comp->action = menu_save_level;
	}
	else if (strcmp(action, "resume_game") == 0)
	{
		comp->action = menu_resume_game;
	}
	else if (strcmp(action, "quit_game") == 0)
	{
		comp->action = menu_quit_game;
	}
	else if (strcmp(action, "load_save_game") == 0)
	{
		comp->action = menu_load_save_game;
	}
	else
	{
		comp->action = menu_do_nothing;
	}

	return comp;
}

void menu_add_to_level(MenuComponent *self)
{
	Vector2D pos;

	pos = menu_manager_get_pos();
	add_to_level(levelEditCode, pos);
}

void menu_change_edit_code(MenuComponent *self)
{
	levelEditCode = atoi(self->action_specification);
}

void menu_save_level(MenuComponent *self)
{
	save_level(thisLevel, self->action_specification);
}

void add_to_level(int code, Vector2D pos)
{
	Uint32 xpos, ypos;

	xpos = (Uint32)pos.x;
	ypos = (Uint32)pos.y;

	thisLevel->tileMap[ypos * thisLevel->levelWidth + xpos] = code;
}

void menu_load_save_game(MenuComponent *self)
{
	SJson *savejs, *playerjs;
	char *location;

	change_save_file(self->action_specification);

	savejs = sj_load(saveFile);
	if (savejs == NULL)
	{
		//If there is no save, this is a new game
		menu_level_transitionNewGame(self);
		return;
	}

	playerjs = sj_object_get_value(savejs, "player");
	if (playerjs == NULL)
	{
		playerjs = sj_object_new();
	}
	sj_save(playerjs, playerFile);

	location = sj_get_string_value(sj_object_get_value(savejs, "location"));
	if (location == NULL || strlen(location) == 0)
	{
		level_transition("defs/room1_1.json");
	}
	else
	{
		level_transition(location);
	}
	resume_game();
}

/*eol@eof*/
