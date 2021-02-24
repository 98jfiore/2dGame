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
* @return NULL on error, or a pointer to a new player entity.
*/
Environ *environment_spawn(Vector2D position);

/**
* @brief Draws the provided environment entity to the current render frame
* @param ent The entity to draw
*/
void environment_draw(Entity *ent);

#endif