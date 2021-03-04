#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "entity.h"

typedef struct
{
	Entity *owner;
	Vector3D rotate;
	Uint32 life;
}Projectile;

/**
* @brief Spawn a projectile entity
* @param position The screen position to spawn the projectile at.
* @param startDir The direction the projectile moves in.
* @param owner The entity who fired the projectile.
* @return NULL on error, or a pointer to a new projectile entity.
*/
Entity *projectile_spawn(Vector2D position, ent_movement_flags startDir, Entity *owner);

/**
* @brief Update the robot entity
* @param self A pointer to the robot entity.
*/
void projectile_update(Entity *self);

/**
* @brief Free the projectile entity
* @param self A pointer to the projectile entity.
*/
void projectile_free(Entity *self);

/**
* @brief Draw the projectile entity
* @param self A pointer to the projectile entity.
*/
void projectile_draw(Entity *self);

#endif
