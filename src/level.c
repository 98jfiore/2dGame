#include <stdlib.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_sprite.h"

#include "level.h"
#include "player.h"

//static Level *thisLevel = {NULL};

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

/*eol@eof*/
