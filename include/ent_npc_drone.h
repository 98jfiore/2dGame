#ifndef __DRONE_H__
#define __DRONE_H__

#include "entity.h"
#include "ent_npc.h"

/**
* @brief Spawn a drone entity
* @param position The screen position to spawn the drone at.
* @param startDir The direction the drone initially moves in.
* @return NULL on error, or a pointer to a new drone entity.
*/
Entity *drone_spawn(Vector2D position, ent_movement_flags startDir);

/**
* @brief Update the robot entity
* @param self A pointer to the robot entity.
*/
void drone_think(Entity *self);

/**
* @brief Free the robot entity
* @param self A pointer to the robot entity.
*/
void drone_free(Entity *self);

#endif
