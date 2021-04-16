#include <stdlib.h>
#include <SDL_keyboard.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "level_save.h"

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

	basejs = sj_object_new();
	sj_object_insert(basejs, "level", leveljs);

	if (basejs == NULL)
	{
		slog("Cannot save null level");
		return;
	}

	slog("Save level %s", saveFile);
	sj_save(basejs, saveFile);
}

/*eol@eof*/