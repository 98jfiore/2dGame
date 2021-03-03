#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include "gfc_vector.h"
#include "gf2d_sprite.h"

#include "attack.h"

typedef struct Inventory_s
{
	char				*name;
	Sprite				*sprite;
	void				(*free)(struct Inventory_s *self);
	Attack				*(*attack)(Entity *owner, ent_movement_flags direction);
	struct Inventory_s	*next;
}Inventory;

/**
* @brief Create a generic inventory item
* @param name The name of the generic item
* @return NULL on error, else a pointer to the inventory item that has been created
*/
Inventory *create_inventory(char *name);

/**
* @brief Create an attack for the user for their inventory
* @return NULL on error, else a pointer to the inventory item that has been created
*/
Inventory *unlock_attack();

/**
* @brief Free an inventory item
* @param inv The inventory item to be freed
*/
void free_inventory(Inventory *inv);

#endif