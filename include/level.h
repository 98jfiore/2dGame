#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gfc_types.h"
#include "gfc_text.h"

#include "gf2d_sprite.h"

typedef enum
{
	TT_Empty,
	TT_Floor,
	TT_Wall,
}TileTypes;

typedef struct
{
	Sprite		*bgImage;			//The background image for the level
	Sprite		*tileSet;			//Sprite for the tileset
	TileTypes	*tileMap;			//Tiles for the level
	Uint32		tileWidth;			//How many pixels wide the tiles are
	Uint32		tileHeight;			//How many pizels tall the tiles are
	Uint32		tileFramesPerLine;	//How many pizels tall the tiles are
	Uint32		levelWidth;			//How many tiles per row the level has
	Uint32		levelHeight;		//How many tiles per column the level has
}Level;

/**
* @brief Allocate and initialize a level
* @return NULL on error, or a pointer to a new level.
*/
Level *level_new();

/**
* @brief Load a level
* @param filename Name of file level is saved on.
* @return NULL on error, or a pointer to the loaded level.
*/
Level *level_load(const char *filename);

/**
* @brief Free up a previously loaded level
* @param level A pointer to the level to free
*/
void level_free(Level *level);

/**
 * @brief Draw a level
 * @param level A pointer to the level to draw
 */
void level_draw(Level *level);

#endif