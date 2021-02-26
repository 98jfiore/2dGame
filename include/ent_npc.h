#ifndef __NPC_H__
#define __NPC_H__

#include "entity.h"

typedef enum{
	NPC_NORTH = 0x1,
	NPC_EAST = 0x2,
	NPC_SOUTH = 0x4,
	NPC_WEST = 0x8,
} npc_flag;

typedef struct
{
	Uint8		movementFlags;
}NPC;

/**
* @brief Spawn a robot entity
* @param position The screen position to spawn the robot at.
* @param startDir The direction the robot initially moves in.
* @return NULL on error, or a pointer to a new robot entity.
*/
Entity *robot_spawn(Vector2D position, npc_flag startDir);

/**
* @brief Update the robot entity
* @param self A pointer to the robot entity.
*/
void robot_think(Entity *self);

/**
* @brief Spawn an npc entity
* @param position The screen position to spawn the npc at.
* @return NULL on error, or a pointer to a new npc entity.
*/
Entity *npc_spawn(Vector2D position);

/**
* @brief Update the npc entity
* @param self A pointer to the npc entity.
*/
void npc_update(Entity *self);

#endif