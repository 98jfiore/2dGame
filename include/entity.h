#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "SDL_rect.h"

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gf2d_sprite.h"

#include "shapes.h"


enum entity_flags {
	ENT_SOLID = 1,
	ENT_DEADLY = 2,
	ENT_DESTRUCTABLE = 4,
	ENT_HITTABLE = 8,
	ENT_PLAYER = 16,
	ENT_NOINVIN = 32,
	ENT_ITEM = 64,
	ENT_UPGRADE = 128,
};

typedef enum{
	MOV_NORTH = 0x1,
	MOV_EAST = 0x2,
	MOV_SOUTH = 0x4,
	MOV_WEST = 0x8,
} ent_movement_flags;

typedef struct Entity_s
{
	Bool		_inuse;
	Uint16		flags;
	Sprite		*sprite;
	Vector2D	position;
	Vector2D	velocity;
	Vector2D	scale;
	Rect		*hitbox;
	float		frame;
	float		frameRate;
	int			frameCount;
	int			baseFrame;
	int			damage;
	void		(*update)(struct Entity_s *self);
	void		(*think)(struct Entity_s *self);
	void		(*draw)(struct Entity_s *self);
	void		(*free)(struct Entity_s *self);
	void		*data;
}Entity;

/**
 * @brief Initialize the entity manager
 * @param max_entities How many concurrent entities to support
 */
void entity_manager_init(Uint32 max_entities);

/**
 * @brief Free all entities in the system and destroy the entity manager
 */
void entity_manager_free();

/**
* @brief Update all entities in the system
*/
void entity_manager_update_entities();

/**
* @brief Draw all entities in the system
*/
void entity_manager_draw_entities();

/**
* @brief Checks to see if entity collided with another entity
* @param self The entity to check collision against.
* @return NULL on no collision or error, or a pointer to an entity self collides with.
*/
Entity *check_collision(Entity *self);

/**
 * @brief Allocate an entity in the entity list and return a pointer to it.
 * @return NULL on error or a pointer to an initialized entity.
 */
Entity *entity_new();

/**
 * @brief Frees the provided entity
 * @param ent The entity to free
 */
void entity_free(Entity *ent);

/**
* @brief Draws the provided entity to the current render frame
* @param ent The entity to draw
*/
void entity_draw(Entity *ent);

/**
* @brief Updates the provided entity to the current state
* @param ent The entity to update
*/
void entity_update(Entity *self);


#endif