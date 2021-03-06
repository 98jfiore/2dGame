#ifndef __ATTACK_H__
#define __ATTACK_H__

#include "gfc_vector.h"
#include "gf2d_sprite.h"

#include "entity.h"
#include "shapes.h"

typedef struct Attack_s
{
	Entity				*owner;
	Rect				*hitbox;
	ent_movement_flags	direction;
	Sprite				*sprite;
	Vector2D			scale;
	float				frame;
	float				frameRate;
	int					lastFrame;
	int					damage;
	void				(*update)(struct Attack_s *self);
	void				(*draw)(struct Attack_s *self);
	void				(*free)(struct Attack_s *self);
}Attack;

/**
* @brief Start an attack
* @param owner The entity whose attack this is
* @param direction The direction this attack is occuring towards
* @return NULL on error, else a pointer to the attack that has started
*/
Attack *start_attack(Entity *owner, ent_movement_flags direction);

/**
* @brief Drop a bomb
* @param owner The entity whose bomb this is
* @return NULL on error, else a pointer to bomb that was dropped
*/
Entity *drop_bomb(Entity *owner);

/**
* @brief Check if the attack has hit any entities
* @param self The attack to check for hits from
* @return NULL on error or no hit, otherwise a pointer to the entity that was hit
*/
Entity *hit_entity(Attack *self);

/**
* @brief Update the attack to current state
* @param self The attack to be updated
*/
void attack_update(Attack *self);

/**
* @brief Draw the attack to current state
* @param self The attack to be drawn
*/
void attack_draw(Attack *self);

/**
* @brief Free the attack to current state
* @param self The attack to be freed
*/
void attack_free(Attack *self);

#endif