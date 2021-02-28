#ifndef __ORB_H__
#define __ORB_H__

#include "entity.h"
#include "ent_npc.h"

typedef struct
{
	Uint8		movementFlags;
	Uint32		pointInCycle;
	Uint32		cycle;
	Rect		*activeHitbox;
	Rect		*inactiveHitbox;
}Orb;

/**
* @brief Spawn an automata entity
* @param position The screen position to spawn the automata at.
* @param startDir The direction the automata initially moves in.
* @param cycle The number of frames before the automata changes its direction.
* @return NULL on error, or a pointer to a new automata entity.
*/
Entity *orb_spawn(Vector2D position, ent_movement_flags startDir, int cycle);

/**
* @brief Update the automata entity
* @param self A pointer to the automata entity.
*/
void orb_think(Entity *self);

/**
* @brief Free the automata entity
* @param self A pointer to the automata entity.
*/
void orb_free(Entity *self);

#endif
