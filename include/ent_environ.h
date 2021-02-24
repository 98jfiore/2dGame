#ifndef __ENVIRON_H__
#define __ENVIRON_H__

#include "entity.h"

enum environment_flags {
	ENV_HITTABLE = 0x1,
	ENV_DEADLY = 0x2,
	ENV_DESTRUCTABLE = 0x4,
};

typedef struct
{
	Uint8 flags;
	Entity *ent;
}Environ;

/**
* @brief Spawn an environment entity
* @param position The screen position to spawn the environmental object at.
* @param spriteSheet The name of the file the sprite we are looking for can be found at.
* @param frameNum The frame of the sprite we are looking for.
* @param spriteWidth The width of sprites in the sprite sheet.
* @param spriteHeight The height of sprites in the sprite sheet.
* @param spriteHeight Frames per line in the sprite sheet.
* @param scale How much to scale the sprite by.
* @return NULL on error, or a pointer to a new player entity.
*/
Environ *environment_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale);

/**
* @brief Draws the provided environment entity to the current render frame
* @param ent The entity to draw
*/
void environment_draw(Entity *ent);

#endif