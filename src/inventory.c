#include "simple_logger.h"

#include "inventory.h"

Inventory *create_inventory(char *name)
{
	Inventory *inv;

	inv = (Inventory *)malloc(sizeof(Inventory));
	inv->name = name;
	inv->sprite = NULL;
	inv->free = NULL;
	inv->attack = NULL;
	inv->next = NULL;
	inv->flags = 0;

	return inv;
}

Inventory *unlock_attack()
{
	Inventory *inv;

	inv = create_inventory("sword");

	inv->attack = start_attack;

	return inv;
}


void free_inventory(Inventory *inv)
{
	if (inv->sprite != NULL) gf2d_sprite_free(inv->sprite);
	if (inv->next == NULL)
	{
		free_inventory(inv->next);
		inv->next = NULL;
	}
	if (inv->free != NULL) inv->free(inv);
	free(inv);
}

/*eol@eof*/
