#ifndef __SWEEPER_H__
#define __SWEEPER_H__

#include "entity.h"
#include "ent_npc.h"

typedef struct
{
	Uint8		movementFlags;
	Uint32		pointInCycle;
	Uint32		cycle;
}Sweeper;

/**
* @brief Spawn a sweeper entity
* @param position The screen position to spawn the sweeper at.
* @param startDir The direction the sweeper initially moves in.
* @param cycle The number of frames before the sweeper changes its direction.
* @return NULL on error, or a pointer to a new sweeper entity.
*/
Entity *sweeper_spawn(Vector2D position, ent_movement_flags startDir, int cycle);

/**
* @brief Update the sweeper entity
* @param self A pointer to the sweeper entity.
*/
void sweeper_think(Entity *self);

/**
* @brief Free the sweeper entity
* @param self A pointer to the sweeper entity.
*/
void sweeper_free(Entity *self);

#endif
