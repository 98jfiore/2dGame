#ifndef __BOSSES_H__
#define __BOSSES_H__

#include "entity.h"
#include "ent_npc.h"

typedef struct
{
	Entity **subents;
	Uint32 num_subents;
	Entity *player;
	char	*tag;
}Boss_One;

/**
* @brief Spawn a boss1 entity
* @param position The screen position to spawn the boss at.
* @return NULL on error, or a pointer to a new boss entity.
*/
Entity *boss1_spawn(Vector2D position);

/**
* @brief Update the automata entity
* @param self A pointer to the automata entity.
*/
void boss1_update(Entity *self);

/**
* @brief Free the automata entity
* @param self A pointer to the automata entity.
*/
void boss1_free(Entity *self);

#endif
