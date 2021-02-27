#ifndef __ENVIRON_H__
#define __ENVIRON_H__

#include "entity.h"

typedef struct
{
	Uint8		movementFlags;
	Uint8		speed;
}MovingEnv;

/**
* @brief Spawn a generic environment entity
* @param position The screen position to spawn the environmental object at.
* @param spriteSheet The name of the file the sprite we are looking for can be found at.
* @param frameNum The frame of the sprite we are looking for.
* @param spriteWidth The width of sprites in the sprite sheet.
* @param spriteHeight The height of sprites in the sprite sheet.
* @param spriteHeight Frames per line in the sprite sheet.
* @param scale How much to scale the sprite by.
* @return NULL on error, or a pointer to a new player entity.
*/
Entity *environment_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale);

/**
* @brief Spawn a wall entity
* @param position The screen position to spawn the environmental object at.
* @param spriteSheet The name of the file the sprite we are looking for can be found at.
* @param frameNum The frame of the sprite we are looking for.
* @param spriteWidth The width of sprites in the sprite sheet.
* @param spriteHeight The height of sprites in the sprite sheet.
* @param spriteHeight Frames per line in the sprite sheet.
* @param scale How much to scale the sprite by.
* @return NULL on error, or a pointer to a new player entity.
*/
Entity *wall_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale);

/**
* @brief Spawn a moving entity
* @param position The screen position to spawn the environmental object at.
* @param spriteSheet The name of the file the sprite we are looking for can be found at.
* @param frameNum The frame of the sprite we are looking for.
* @param spriteWidth The width of sprites in the sprite sheet.
* @param spriteHeight The height of sprites in the sprite sheet.
* @param spriteHeight Frames per line in the sprite sheet.
* @param scale How much to scale the sprite by.
* @param speed The rate the wall will move.
* @param direction The initial direction the wall will move.
* @return NULL on error, or a pointer to a new player entity.
*/
Entity *moving_wall_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale, int speed, ent_movement_flags direction);

/**
* @brief Update a moving wall entity
* @param self The wall that is to be updated.
*/
void moving_wall_update(Entity *self);

/**
* @brief Free a moving wall entity
* @param self The wall that is to be updated.
*/
void moving_wall_free(Entity *self);

/**
* @brief Spawn a pit entity
* @param position The screen position to spawn the environmental object at.
* @param spriteSheet The name of the file the sprite we are looking for can be found at.
* @param frameNum The frame of the sprite we are looking for.
* @param spriteWidth The width of sprites in the sprite sheet.
* @param spriteHeight The height of sprites in the sprite sheet.
* @param spriteHeight Frames per line in the sprite sheet.
* @param scale How much to scale the sprite by.
* @return NULL on error, or a pointer to a new player entity.
*/
Entity *pit_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale);

/**
* @brief Spawn a pit entity
* @param position The screen position to spawn the environmental object at.
* @param spriteSheet The name of the file the sprite we are looking for can be found at.
* @param frameNum The frame of the sprite we are looking for.
* @param spriteWidth The width of sprites in the sprite sheet.
* @param spriteHeight The height of sprites in the sprite sheet.
* @param spriteHeight Frames per line in the sprite sheet.
* @param scale How much to scale the sprite by.
* @return NULL on error, or a pointer to a new player entity.
*/
Entity *spike_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale);

/**
* @brief Draws the provided environment entity to the current render frame
* @param ent The entity to draw
*/
void environment_draw(Entity *ent);

#endif