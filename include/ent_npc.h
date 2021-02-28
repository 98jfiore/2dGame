#ifndef __NPC_H__
#define __NPC_H__

#include "entity.h"

typedef struct
{
	Uint8		movementFlags;
}NPC;

/**
* @brief Spawn an npc entity
* @param position The screen position to spawn the npc at.
* @return NULL on error, or a pointer to a new npc entity.
*/
Entity *npc_spawn(Vector2D position);

/**
* @brief Update the npc entity
* @param self A pointer to the npc entity.
*/
void npc_update(Entity *self);

#endif