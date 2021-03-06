#ifndef __UPGRADE_H__
#define __UPGRADE_H__

#include "SDL_stdinc.h"

#include "entity.h"

typedef struct
{
	SDL_bool	(*action)(Entity *self, Entity *ent);
	char		*tag;
}Upgrade;

/**
* @brief Spawn an upgrade entity
* @param position The screen position to spawn the upgrade item at.
* @param tag The tag of the upgrade item to spawn.
* @return NULL on error, or a pointer to a new upgrade item entity.
*/
Entity *upgrade_spawn(Vector2D position, char *tag);

/**
* @brief Free an item entity
* @param ent A pointer to the entity to free.
*/
void upgrade_free(Entity *ent);

/**
* @brief Spawn a health item entity
* @param position The screen position to spawn the health item at.
* @param tag The tag of the upgrade item to spawn.
* @return NULL on error, or a pointer to a new health item entity.
*/
Entity *health_spawn(Vector2D position, char *tag);

/**
* @brief When a player hits this upgrade item, do something
* @param self A pointer to the item entity.
* @param ent The entity to be acted on.
*/
SDL_bool health_action(Entity *self, Entity *ent);

/**
* @brief Spawn a sword item entity
* @param position The screen position to spawn the sword item at.
* @param tag The tag of the upgrade item to spawn.
* @return NULL on error, or a pointer to a new sword item entity.
*/
Entity *sword_spawn(Vector2D position, char *tag);

/**
* @brief When a player hits this upgrade item, do something
* @param self A pointer to the item entity.
* @param ent The entity to be acted on.
*/
SDL_bool sword_action(Entity *self, Entity *ent);

/**
* @brief Spawn a sword upgrade item entity
* @param position The screen position to spawn the sword item at.
* @param tag The tag of the upgrade item to spawn.
* @return NULL on error, or a pointer to a new sword item entity.
*/
Entity *swordUpgrade_spawn(Vector2D position, char *tag);

/**
* @brief When a player hits this upgrade item, do something
* @param self A pointer to the item entity.
* @param ent The entity to be acted on.
*/
SDL_bool swordUpgrade_action(Entity *self, Entity *ent);

/**
* @brief Spawn a key upgrade item entity
* @param position The screen position to spawn the key item at.
* @param tag The tag of the upgrade item to spawn.
* @return NULL on error, or a pointer to a new key item entity.
*/
Entity *key_spawn(Vector2D position, char *tag);

/**
* @brief When a player hits this upgrade item, do something
* @param self A pointer to the item entity.
* @param ent The entity to be acted on.
*/
SDL_bool key_action(Entity *self, Entity *ent);

/**
* @brief Spawn a bomb upgrade item entity
* @param position The screen position to spawn the sword item at.
* @param tag The tag of the upgrade item to spawn.
* @return NULL on error, or a pointer to a new bomb item entity.
*/
Entity *bombUpgrade_spawn(Vector2D position, char *tag);

/**
* @brief When a player hits this upgrade item, do something
* @param self A pointer to the item entity.
* @param ent The entity to be acted on.
*/
SDL_bool bombUpgrade_action(Entity *self, Entity *ent);

#endif