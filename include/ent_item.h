#ifndef __ITEM_H__
#define __ITEM_H__

#include "entity.h"

typedef struct
{
	void	(*action)(Entity *self, Entity *ent);
}Item;

/**
* @brief Spawn an item entity
* @param position The screen position to spawn the item at.
* @return NULL on error, or a pointer to a new item entity.
*/
Entity *item_spawn(Vector2D position);

/**
* @brief Free an item entity
* @param ent A pointer to the entity to free.
*/
void item_free(Entity *ent);

/**
* @brief Spawn an healing item entity
* @param position The screen position to spawn the healing item at.
* @return NULL on error, or a pointer to a new healing item entity.
*/
Entity *healing1_spawn(Vector2D position);

/**
* @brief When a player hits this item, do something
* @param self A pointer to the item entity.
* @param ent The entity to be acted on.
*/
void healing1_action(Entity *self, Entity *ent);

#endif