#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gfc_types.h"
#include "gfc_text.h"

#include "gf2d_sprite.h"

#include "entity.h"

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
	float		scaleAmount;
	Vector2D	scaleVec;
	Vector2D	scalePos;
}Level;

typedef struct
{
	char *nextLevel;
	char *nextPos;
	Level *currentLevel;
}Transition;

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
* @brief Load a level
* @param filename Name of file level is saved on.
* @param player An entity pointing to an already loaded player
* @param nextPlayerPos A string containing the title of the next next player position
* @return NULL on error, or a pointer to the loaded level.
*/
Level *level_loadWithPlayer(const char *filename, Entity *player, char *nextPlayerPos);

/**
* @brief Load the basic information from a level json without any player loading
* @param filename Name of file level is saved on.
* @return NULL on error, or a pointer to the loaded level.
*/
Level *level_jsonload(const char *filename);

/**
* @brief Unload a level=
* @param level A pointer to the current level.
* @return NULL on error or no player, else return a pointer to the player entity.
*/
Entity *level_unload(Level *level);

/**
* @brief Unload a level=
* @param nextLevel A string containing the title of the next level
* @param nextPos A string containing the title of the next next player position
* @param currentLevel A pointer to the current level.
*/
void level_transition(const char *nextLevel, char *nextPos, Level *currentLevel);

/**
* @brief Free up a previously loaded level
* @param level A pointer to the level to free
*/
void level_free(Level *level);

/**
 * @brief Draw a level
 */
void level_draw();

/**
* @brief Spawn a transition entity
* @param position The screen position to spawn the transitoin object at.
* @param spriteSheet The name of the file the sprite we are looking for can be found at.
* @param frameNum The frame of the sprite we are looking for.
* @param spriteWidth The width of sprites in the sprite sheet.
* @param spriteHeight The height of sprites in the sprite sheet.
* @param spriteHeight Frames per line in the sprite sheet.
* @param scale How much to scale the sprite by.
* @param nextLevel The name of the next level you want to transition to.
* @param nextPos The title of the position to transition to.
* @param currentLevel A pointer to the current level.
* @return NULL on error, or a pointer to a new transition entity.
*/
Entity *transition_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale, char *nextLevel, char *nextPos, Level *currentLevel);

/**
* @brief Update a moving wall entity
* @param self The wall that is to be updated.
*/
void transition_update(Entity *self);

/**
* @brief Free a transition entity
* @param self The door that is to be updated.
*/
void transition_free(Entity *self);

#endif