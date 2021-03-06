#ifndef __BOMB_H__
#define __BOMB_H__

#include "entity.h"

typedef struct
{
	Entity *owner;
	Uint32 timer;
}Bomb;

/**
* @brief Spawn a bomb entity
* @param position The screen position to spawn the bomb at.
* @param owner The entity who dropped the bomb.
* @return NULL on error, or a pointer to a new bomb entity.
*/
Entity *bomb_spawn(Vector2D position, Entity *owner);

/**
* @brief Update the bomb entity
* @param self A pointer to the bomb entity.
*/
void bomb_update(Entity *self);

/**
* @brief Explode the bomb entity
* @param self A pointer to the bomb entity.
*/
void bomb_explode(Entity *self);

/**
* @brief Spawn a bomb entity
* @param position The screen position to spawn the explosion at.
* @param owner The entity who dropped the bomb.
* @return NULL on error, or a pointer to a new explosion entity.
*/
Entity *explosion_spawn(Vector2D position, Entity *owner);

/**
* @brief Update the explosion entity
* @param self A pointer to the bomb entity for the explosion.
*/
void explosion_update(Entity *self);

/**
* @brief Draw the explosion entity
* @param self A pointer to the bomb entity for the explosion.
*/
void explosion_draw(Entity *self);

/**
* @brief Free the projectile entity
* @param self A pointer to the projectile entity.
*/
void bomb_free(Entity *self);

#endif
