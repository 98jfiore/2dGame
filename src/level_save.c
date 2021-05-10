#include <stdlib.h>
#include <SDL_keyboard.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "level_save.h"

static SJson *enemiesjs = NULL;

void save_level(Level *level, char *saveFile)
{
	SJson *leveljs, *playerspawnjs, *basejs;
	SJson *tile_array, *tile_row;
	int i, j, startx = 0, starty = 0;

	if (level == NULL || saveFile == NULL)
	{
		slog("Cannot save null level");
		return;
	}

	leveljs = sj_load("defs/base_jsonLevel.json");

	tile_array = sj_array_new();
	for (i = 0; i < level->levelHeight; ++i)
	{
		tile_row = sj_array_new();
		for (j = 0; j < level->levelWidth; ++j)
		{
			sj_array_append(tile_row, sj_new_int(level->tileMap[i*level->levelWidth + j]));
			if (startx == 0 && starty == 0 && level->tileMap[i*level->levelWidth + j] == 1)
			{
				startx = j;
				starty = i;
			}
		}
		sj_array_append(tile_array, tile_row);
	}

	sj_object_insert(leveljs, "tileMap", tile_array);
	
	playerspawnjs = sj_object_new();
	if (playerspawnjs == NULL)
	{
		slog("WHAT");
	}
	sj_object_insert(playerspawnjs, "x", sj_new_int(startx));
	sj_object_insert(playerspawnjs, "y", sj_new_int(starty));
	sj_object_insert(leveljs, "player spawn", playerspawnjs);

	if (enemiesjs != NULL)
	{
		sj_object_insert(leveljs, "enemies", enemiesjs);
	}

	basejs = sj_object_new();

	if (basejs == NULL)
	{
		slog("Cannot save null level");
		return;
	}

	sj_object_insert(basejs, "level", leveljs);

	slog("Save level %s", saveFile);
	sj_save(basejs, saveFile);
}

void clear_enemies_js()
{
	if (enemiesjs == NULL)
	{
		return;
	}
	else
	{
		sj_free(enemiesjs);
		enemiesjs = NULL;
	}
}



void add_enemy_customLevel(int enemyCode, int xpos, int ypos, const char *dir)
{
	SJson *enemy;

	if (enemiesjs == NULL)
	{
		enemiesjs = sj_array_new();
	}

	enemy = sj_object_new();
	
	sj_object_insert(enemy, "x", sj_new_int(xpos));
	sj_object_insert(enemy, "y", sj_new_int(ypos));
	sj_object_insert(enemy, "startDir", sj_new_str(dir));
	if (enemyCode == 0)
	{
		sj_object_insert(enemy, "type", sj_new_str("robot"));
	}
	else if (enemyCode == 1)
	{
		sj_object_insert(enemy, "type", sj_new_str("drone"));
	}
	else if (enemyCode == 2)
	{
		sj_object_insert(enemy, "type", sj_new_str("sweeper"));
		sj_object_insert(enemy, "cycle", sj_new_int(80));
	}
	else if (enemyCode == 3)
	{
		sj_object_insert(enemy, "type", sj_new_str("automata"));
		sj_object_insert(enemy, "cycle", sj_new_int(80));
	}
	else if (enemyCode == 4)
	{
		sj_object_insert(enemy, "type", sj_new_str("orb"));
		sj_object_insert(enemy, "cycle", sj_new_int(180));
	}
	else
	{
		sj_object_insert(enemy, "type", sj_new_str("none"));
	}

	sj_array_append(enemiesjs, enemy);
}


/*eol@eof*/