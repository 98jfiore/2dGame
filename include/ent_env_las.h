#ifndef __LASER_H__
#define __LASER_H__

#include "entity.h"
#include "ent_environ.h"

typedef struct
{
	Uint8		dirFlag;
	Uint32		pointInCycle;
	Uint32		cycle;
	Rect		*hitbox;
}Laser;


/**
* @brief Spawn a laser entity
* @param position The screen position to spawn the laser at.
* @param spriteSheet The name of the file the sprite we are looking for can be found at.
* @param frameNum The frame of the sprite we are looking for.
* @param spriteWidth The width of sprites in the sprite sheet.
* @param spriteHeight The height of sprites in the sprite sheet.
* @param spriteHeight Frames per line in the sprite sheet.
* @param scale How much to scale the sprite by.
* @param cycle How many frames is the laser active or inactive for.
* @param orientation The direction the laser will face.
* @return NULL on error, or a pointer to a new player entity.
*/
Entity *laser_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale, int cycle, ent_movement_flags orientation);

/**
* @brief Spawn a laser entity
* @param position The screen position to spawn the laser at.
* @param spriteSheet The name of the file the sprite we are looking for can be found at.
* @param frameNum The frame of the sprite we are looking for.
* @param laserFrameNum The frame of the sprite we are looking for for the laser itself.
* @param spriteWidth The width of sprites in the sprite sheet.
* @param spriteHeight The height of sprites in the sprite sheet.
* @param spriteHeight Frames per line in the sprite sheet.
* @param scale How much to scale the sprite by.
* @param cycle How many frames is the laser active or inactive for.
* @param range How far the lasers go.
* @param orientation The direction the laser will face.
* @return NULL on error, or a pointer to a new player entity.
*/
Entity *laserBase_spawn(Vector2D position, char *spriteSheet, int frameNum, int laserFrameNum, int spriteWidth, int spriteHeight, int fpl, int scale, int cycle, int range, ent_movement_flags orientation);

/**
* @brief Update a laser entity
* @param self The laser that is to be updated.
*/
void laser_update(Entity *self);

/**
* @brief Draw a laser entity
* @param self The laser that is to be drawn.
*/
void laser_draw(Entity *self);

/**
* @brief Free a laser entity
* @param self The laser that is to be freed.
*/
void laser_free(Entity *self);

#endif