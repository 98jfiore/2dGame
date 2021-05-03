#ifndef __BOSSES_H__
#define __BOSSES_H__

#include "entity.h"
#include "gf2d_particles.h"
#include "particles.h"
#include "ent_npc.h"

typedef struct
{
	Entity **subents;
	Uint16	flags;
	Uint32	cycle;
	int		head1_cycle;
	int		head2_cycle;
	Uint8	chase_head;
	Uint32 num_subents;
	Entity *player;
	char	*tag;
	ParticleEmitter *pe;
	ParticleSource *ps;
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
* @brief Draw the boss1 entity
* @param self A pointer to the automata entity.
*/
void boss1_draw(Entity *self);

/**
* @brief Free the automata entity
* @param self A pointer to the automata entity.
*/
void boss1_free(Entity *self);


/**
* @brief Update hitboxes of boss1
* @param self A pointer to the boss1 entity.
*/
void boss1_updateHitboxes(Entity *self);

#endif
