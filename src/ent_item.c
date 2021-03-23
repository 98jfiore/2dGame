#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "ent_item.h"
#include "player.h"
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

	return NULL;
}

void healing1_action(Entity *self, Entity *ent)
{
	Player *player;

	if (ent == NULL) return;
	
	if (ent->flags & ENT_PLAYER)
	{
		player = (Player *)ent->data;
		if (player->health < player->maxhealth)
		{
			player->health++;
			entity_free(self);
		}
	}

}

Entity *healing2_spawn(Vector2D position)
{
	Entity *ent;
	Item *item;
	Rect *hitbox;

	ent = item_spawn(position);
	item = (Item *)ent->data;

	ent->sprite = gf2d_sprite_load_all("images/items.png", 16, 16, 5);
	ent->frameCount = 5;
	ent->frameRate = 0;
	ent->baseFrame = 1;
	ent->frame = 1;
	ent->scale = vector2d(2, 2);

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x + 1;
	hitbox->y = position.y + 1;
	hitbox->width = ent->sprite->frame_w * 2 - 2;
	hitbox->height = ent->sprite->frame_h * 2 - 2;
	ent->hitbox = hitbox;

	item->action = healing2_action;

	return NULL;
}

void healing2_action(Entity *self, Entity *ent)
{
	Player *player;

	if (ent == NULL) return;

	if (ent->flags & ENT_PLAYER)
	{
		player = (Player *)ent->data;
		if (player->health < player->maxhealth)
		{
			player->health += 2;
			if (player->health > player->maxhealth) player->health = player->maxhealth;
			entity_free(self);
		}
	}
}

Entity *healing3_spawn(Vector2D position)
{
	Entity *ent;
	Item *item;
	Rect *hitbox;

	ent = item_spawn(position);
	item = (Item *)ent->data;

	ent->sprite = gf2d_sprite_load_all("images/items.png", 16, 16, 5);
	ent->frameCount = 5;
	ent->frameRate = 0;
	ent->baseFrame = 2;
	ent->frame = 2;
	ent->scale = vector2d(2, 2);

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x + 1;
	hitbox->y = position.y + 1;
	hitbox->width = ent->sprite->frame_w * 2 - 2;
	hitbox->height = ent->sprite->frame_h * 2 - 2;
	ent->hitbox = hitbox;

	item->action = healing3_action;

	return NULL;
}

void healing3_action(Entity *self, Entity *ent)
{
	Player *player;

	if (ent == NULL) return;

	if (ent->flags & ENT_PLAYER)
	{
		player = (Player *)ent->data;
		if (player->health < player->maxhealth)
		{
			player->health = player->maxhealth;
			entity_free(self);
		}
	}

}

Entity *invin1_spawn(Vector2D position)
{
	Entity *ent;
	Item *item;
	Rect *hitbox;

	ent = item_spawn(position);
	item = (Item *)ent->data;

	ent->sprite = gf2d_sprite_load_all("images/items.png", 16, 16, 5);
	ent->frameCount = 5;
	ent->frameRate = 0;
	ent->baseFrame = 3;
	ent->frame = 3;
	ent->scale = vector2d(2, 2);

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x + 1;
	hitbox->y = position.y + 1;
	hitbox->width = ent->sprite->frame_w * 2 - 2;
	hitbox->height = ent->sprite->frame_h * 2 - 2;
	ent->hitbox = hitbox;

	item->action = invin1_action;

	return NULL;
}

void invin1_action(Entity *self, Entity *ent)
{
	Player *player;

	if (ent == NULL) return;

	if (ent->flags & ENT_PLAYER)
	{
		player = (Player *)ent->data;
		if ((player->flags & PLR_INVIN) == 0)
		{
			player->flags = player->flags | PLR_INVIN;
		}
		player->iframesRemaining = 300;
		entity_free(self);
	}

}

Entity *invin2_spawn(Vector2D position)
{
	Entity *ent;
	Item *item;
	Rect *hitbox;

	ent = item_spawn(position);
	item = (Item *)ent->data;

	ent->sprite = gf2d_sprite_load_all("images/items.png", 16, 16, 5);
	ent->frameCount = 5;
	ent->frameRate = 0;
	ent->baseFrame = 4;
	ent->frame = 4;
	ent->scale = vector2d(2, 2);

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x + 1;
	hitbox->y = position.y + 1;
	hitbox->width = ent->sprite->frame_w * 2 - 2;
	hitbox->height = ent->sprite->frame_h * 2 - 2;
	ent->hitbox = hitbox;

	item->action = invin2_action;

	return NULL;
}

void invin2_action(Entity *self, Entity *ent)
{
	Player *player;

	if (ent == NULL) return;

	if (ent->flags & ENT_PLAYER)
	{
		player = (Player *)ent->data;
		if ((player->flags & PLR_INVIN) == 0)
		{
			player->flags = player->flags | PLR_INVIN;
		}
		player->iframesRemaining = 600;
		entity_free(self);
	}

}



/*eol@eof*/