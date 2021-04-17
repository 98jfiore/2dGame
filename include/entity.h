#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "SDL_rect.h"

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gf2d_sprite.h"

#include "shapes.h"
//#include "attack.h"


enum entity_flags {
	ENT_SOLID = 1,
	ENT_DEADLY = 2,
	ENT_DESTRUCTABLE = 4,
	ENT_HITTABLE = 8,
	ENT_PLAYER = 16,
	ENT_NOINVIN = 32,
	ENT_ITEM = 64,
	ENT_UPGRADE = 128,
	ENT_LOCKED = 256,
	ENT_TRANSITION = 512,
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
* @brief Deletes all entities that are not the player
* @return NULL on error or no player, or a pointer to the player entity
*/
Entity *delete_notplayer();

/**
* @brief Get the player
* @return NULL on error or no player, or a pointer to the player entity
*/
Entity *get_player();

/**
* @brief Deletes all entities
* @return NULL on error or no player, or a pointer to the player entity
*/
void delete_all_ent();

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
* @brief Checks to see if entity collided with a player
* @param self The entity to check collision against.
* @return SDL_TRUE on collision with player, SDL_FALSE on error or no collision
*/
SDL_bool check_collision_with_player(Entity *self);

/**
* @brief Checks to see if attack hit any valid entities
* @param atk The hitbox of the attack to check for hits.
* @param owner The owner of the attack.
* @return NULL on no hit or error, or a pointer to an entity atk hits.
*/
Entity *check_attackHit(Rect *atk, Entity *owner);

/**
* @brief Explosion does damage to entities it hits.
* @param atk The hitbox of the explosion to check for hits.
* @param owner The owner of the attack.
* @return NULL on no hit or error, or a pointer to an entity atk hits.
*/
void check_explosionHit(Rect *explosion, Entity *owner);

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

/**
* @brief Do some damage to the player entity
* @param player A pointer to the player entity.
* @param damage The amound of damage to do to the player.
*/
void player_doDamage(Entity *player, int damage);


#endif