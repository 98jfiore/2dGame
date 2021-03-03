#ifndef __UPGRADE_H__
#define __UPGRADE_H__

#include "entity.h"

typedef struct
{
	void	(*action)(Entity *self, Entity *ent);
}Upgrade;

/**
* @brief Spawn an upgrade entity
* @param position The screen position to spawn the upgrade item at.
* @return NULL on error, or a pointer to a new upgrade item entity.
*/
Entity *upgrade_spawn(Vector2D position);

/**
* @brief Free an item entity
* @param ent A pointer to the entity to free.
*/
void upgrade_free(Entity *ent);

/**
* @brief Spawn a health item entity
* @param position The screen position to spawn the health item at.
* @return NULL on error, or a pointer to a new health item entity.
*/
Entity *health_spawn(Vector2D position);

/**
* @brief When a player hits this upgrade item, do something
* @param self A pointer to the item entity.
* @param ent The entity to be acted on.
*/
void health_action(Entity *self, Entity *ent);

/**
* @brief Spawn a sword item entity
* @param position The screen position to spawn the sword item at.
* @return NULL on error, or a pointer to a new sword item entity.
*/
Entity *sword_spawn(Vector2D position);

/**
* @brief When a player hits this upgrade item, do something
* @param self A pointer to the item entity.
* @param ent The entity to be acted on.
*/
void sword_action(Entity *self, Entity *ent);

#endif