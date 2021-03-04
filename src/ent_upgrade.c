#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "ent_upgrade.h"
#include "player.h"
#include "inventory.h"
#include "shapes.h"


Entity *upgrade_spawn(Vector2D position, char *tag)
{
	Entity *ent;
	Upgrade *upgrade;

	ent = entity_new();
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->flags = ENT_UPGRADE;

	upgrade = (Upgrade *)malloc(sizeof(Upgrade));
	ent->data = upgrade;
	upgrade->tag = malloc(strlen(tag) + 1);
	strcpy(upgrade->tag, tag);
	//upgrade->tag = tag

	ent->free = upgrade_free;

	return ent;
}

void upgrade_free(Entity *self)
{
	Upgrade *upgrade;

	upgrade = (Upgrade *)self->data;
	free(upgrade);
}

Entity *health_spawn(Vector2D position, char *tag)
{
	Entity *ent;
	Upgrade *upgrade;
	Rect *hitbox;

	ent = upgrade_spawn(position, tag);
	upgrade = (Upgrade *)ent->data;

	ent->sprite = gf2d_sprite_load_all("images/morehealth.png", 16, 16, 1);
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

	upgrade->action = health_action;

	ent->data = upgrade;

	return ent;
}

SDL_bool health_action(Entity *self, Entity *ent)
{
	Player *player;

	if (ent == NULL) return SDL_FALSE;

	if (ent->flags & ENT_PLAYER)
	{
		player = (Player *)ent->data;
		player->maxhealth++;
		entity_free(self);
		return SDL_TRUE;
	}

	return SDL_FALSE;
}

Entity *sword_spawn(Vector2D position, char *tag)
{
	Entity *ent;
	Upgrade *upgrade;
	Rect *hitbox;

	ent = upgrade_spawn(position, tag);
	upgrade = (Upgrade *)ent->data;

	ent->sprite = gf2d_sprite_load_all("images/sword.png", 16, 16, 1);
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

	upgrade->action = sword_action;

	return ent;
}

SDL_bool sword_action(Entity *self, Entity *ent)
{
	Player *player;
	Inventory *inv;

	if (ent == NULL || self == NULL) return SDL_FALSE;

	if (ent->flags & ENT_PLAYER)
	{
		player = (Player *)ent->data;
		if (player == NULL)
		{
			slog("Cannot unlock sword for nonexistent player");
			return;
		}
		
		inv = unlock_attack();
		inv->next = player->inventory;
		player->inventory = inv;
		entity_free(self);

		return SDL_TRUE;
	}

	return SDL_FALSE;
}

Entity *swordUpgrade_spawn(Vector2D position, char *tag)
{
	Entity *ent;
	Upgrade *upgrade;
	Rect *hitbox;

	ent = upgrade_spawn(position, tag);
	upgrade = (Upgrade *)ent->data;

	ent->sprite = gf2d_sprite_load_all("images/swordbeam.png", 16, 16, 1);
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

	upgrade->action = swordUpgrade_action;

	return ent;
}

SDL_bool swordUpgrade_action(Entity *self, Entity *ent)
{
	Player *player;
	Inventory *inv;

	if (ent == NULL || self == NULL) return SDL_FALSE;

	if (ent->flags & ENT_PLAYER)
	{
		player = (Player *)ent->data;
		if (player == NULL)
		{
			slog("Cannot upgrade sword for nonexistent player");
			return SDL_FALSE;
		}

		inv = player->inventory;
		while (inv != NULL)
		{
			if (strcmp(inv->name, "sword") == 0)
			{
				inv->flags = inv->flags | INV_UPGRADED;
				entity_free(self);

				return SDL_TRUE;
			}
			inv = inv->next;
		}
	}

	return SDL_FALSE;
}

Entity *key_spawn(Vector2D position, char *tag)
{
	Entity *ent;
	Upgrade *upgrade;
	Rect *hitbox;

	ent = upgrade_spawn(position, tag);
	upgrade = (Upgrade *)ent->data;

	ent->sprite = gf2d_sprite_load_all("images/key.png", 16, 16, 1);
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

	upgrade->action = key_action;

	return ent;
}

SDL_bool key_action(Entity *self, Entity *ent)
{
	Player *player;
	Inventory *inv, *player_inv;
	Upgrade *upgrade;

	if (ent == NULL || self == NULL) return SDL_FALSE;

	if (ent->flags & ENT_PLAYER)
	{
		player = (Player *)ent->data;
		if (player == NULL)
		{
			slog("Cannot unlock key for nonexistent player");
			return SDL_FALSE;
		}

		//Create Key inventory item
		upgrade = (Upgrade *)self->data;
		if (upgrade == NULL)
		{
			slog("Item is not an upgrade");
			return SDL_FALSE;
		}
		inv = unlock_key(upgrade->tag);

		//Add key to back of player's inventory
		player_inv = player->inventory;
		if (player_inv == NULL)
		{
			player->inventory = inv;
		}
		else
		{
			while (player_inv->next != NULL)
			{
				player_inv = player_inv->next;
			}
			player_inv->next = inv;
		}

		entity_free(self);
		return SDL_TRUE;
	}
	return SDL_FALSE;
}

/*eol@eof*/