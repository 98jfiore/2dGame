#ifndef __ENVIRON_H__
#define __ENVIRON_H__

#include "entity.h"

/**
* @brief Spawn a generic environment entity
* @param position The screen position to spawn the environmental object at.
* @param spriteSheet The name of the file the sprite we are looking for can be found at.
* @param frameNum The frame of the sprite we are looking for.
* @param spriteWidth The width of sprites in the sprite sheet.
* @param spriteHeight The height of sprites in the sprite sheet.
* @param spriteHeight Frames per line in the sprite sheet.
* @param scale How much to scale the sprite by.
* @return NULL on error, or a pointer to a new player entity.
*/
Entity *environment_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale);

/**
* @brief Spawn a pit entity
* @param position The screen position to spawn the environmental object at.
* @param spriteSheet The name of the file the sprite we are looking for can be found at.
* @param frameNum The frame of the sprite we are looking for.
* @param spriteWidth The width of sprites in the sprite sheet.
* @param spriteHeight The height of sprites in the sprite sheet.
* @param spriteHeight Frames per line in the sprite sheet.
* @param scale How much to scale the sprite by.
* @return NULL on error, or a pointer to a new player entity.
*/
Entity *pit_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale);

/**
* @brief Draws the provided environment entity to the current render frame
* @param ent The entity to draw
*/
void environment_draw(Entity *ent);

#endif