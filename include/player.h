#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"
#include "shapes.h"

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

#endif