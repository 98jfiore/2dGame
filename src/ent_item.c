#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "ent_item.h"
#include "shapes.h"


Entity *item_spawn(Vector2D position)
{
	Entity *ent;
	Item *item;

	ent = entity_new();
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}
	
	vector2d_copy(ent->position, position);
	
	ent->flags = ENT_ITEM;

	item = (Item *)malloc(sizeof(Item));
	ent->data = item;

	ent->free = item_free;

	return ent;
}

void item_free(Entity *self)
{
	Item *item;
	
	item = (Item *)self->data;
	free(item);
}

Entity *healing1_spawn(Vector2D position)
{
	Entity *ent;
	Item *item;
	Rect *hitbox;

	ent = item_spawn(position);
	item = (Item *)ent->data;

	ent->sprite = gf2d_sprite_load_all("images/items.png", 16, 16, 5);
	ent->frameCount = 1;
	ent->frameRate = 0;
	ent->baseFrame = 0;
	ent->scale = vector2d(2, 2);
	
	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x + 1;
	hitbox->y = position.y + 1;
	hitbox->width = ent->sprite->frame_w * 2 - 2;
	hitbox->height = ent->sprite->frame_h * 2 - 2;
	ent->hitbox = hitbox;

	item->action = healing1_action;

	//item_free(ent);

	return NULL;
}

void healing1_action(Entity *ent)
{
	return;
}


/*eol@eof*/