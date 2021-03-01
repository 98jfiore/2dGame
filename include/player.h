#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"
#include "shapes.h"

typedef enum{
	PLR_ALIVE = 1,
	PLR_DEAD = 2,
	PLR_INVIN = 4,
} player_flags;

typedef struct
{
	Uint8	health;
	Uint8	flags; 
	Uint32	iframesRemaining;
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

#endif