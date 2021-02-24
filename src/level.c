#include <stdlib.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_sprite.h"

#include "level.h"

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

		level->tileSet = gf2d_sprite_load_all(
			(char *)string,
			level->tileWidth,
			level->tileHeight,
			level->tileFramesPerLine);
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
			sj_get_integer_value(sj_array_get_nth(row, i), &level->tileMap[tileIndex++]);
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
		gf2d_sprite_draw(
			level->tileSet,
			vector2d((i % level->levelWidth) * level->tileSet->frame_w, (i / level->levelHeight) * level->tileSet->frame_h),
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			level->tileMap[i] - 1);
	}
}

/*eol@eof*/
