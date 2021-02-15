#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gf2d_sprite.h"

typedef struct
{
	Bool _inuse;
	Sprite *sprite;
	Vector2D position;
	float frame;
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


#endif