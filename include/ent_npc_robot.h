#ifndef __ROBOT_H__
#define __ROBOT_H__

#include "entity.h"
#include "ent_npc.h"

/**
* @brief Spawn a robot entity
* @param position The screen position to spawn the robot at.
* @param startDir The direction the robot initially moves in.
* @return NULL on error, or a pointer to a new robot entity.
*/
Entity *robot_spawn(Vector2D position, ent_movement_flags startDir);

/**
* @brief Update the robot entity
* @param self A pointer to the robot entity.
*/
void robot_think(Entity *self);

/**
* @brief Free the robot entity
* @param self A pointer to the robot entity.
*/
void robot_free(Entity *self);

#endif
