#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "simple_json.h"

#include "entity.h"
#include "shapes.h"
#include "attack.h"
#include "inventory.h"

typedef enum{
	PLR_ALIVE = 1,
	PLR_DEAD = 2,
	PLR_INVIN = 4,
	PLR_TOATTACK = 8,
	PLR_ATTACKING = 16,
} player_flags;

typedef struct
{
	Uint8		maxhealth;
	Uint8		health;
	Uint8		flags; 
	Uint32		iframesRemaining;
	Attack		*attack;
	Uint32		attackWait;
	Inventory	*inventory;
}Player;

/**
 * @brief Spawn a player entity
 * @param position The screen position to spawn the player at.
 * @return NULL on error, or a pointer to a new player entity.
 */
Entity *player_spawn(Vector2D position);

/**
* @brief Process all the thinking the player has to do
* @param self A pointer to the player entity.
*/
void player_think(Entity *self);

/**
* @brief Update the player entity
* @param self A pointer to the player entity.
*/
void player_update(Entity *self);

/**
* @brief Draw the player entity
* @param self A pointer to the player entity.
*/
void player_draw(Entity *self);

/**
* @brief Save the player's state to a json
* @param self A pointer to the player entity to be saved.
* @param fileName The name of the file to save the player to.
*/
void save_player(Entity *self, char *fileName);

/**
* @brief Save the player's state to a json
* @param fileName The name of the file to save the player to.
* @param upgradeTag The name of the upgrade to save that the player received.
*/
void save_playerUpgrade(char *fileName, char *upgradeTag);

/**
* @brief Load a player state from a json file
* @param ent A pointer to the entity to load information onto.
* @param fileName The name of the file to load the player from.
*/
void load_player(Entity *ent, char *fileName);

/**
* @brief Load a player's inventory from a json
* @param ent A pointer to the entity to load information onto.
* @param save A Json object with information for the player's inventory in it.
*/
void load_player_inventory(Entity *ent, SJson *save);

#endif