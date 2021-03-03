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

void health_action(Entity *self, Entity *ent)
{
	Player *player;

	if (ent == NULL) return;

	if (ent->flags & ENT_PLAYER)
	{
		player = (Player *)ent->data;
		player->maxhealth++;
		entity_free(self);
	}

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

void sword_action(Entity *self, Entity *ent)
{
	Player *player;
	Inventory *inv;

	if (ent == NULL || self == NULL) return;

	if (ent->flags & ENT_PLAYER)
	{
		player = (Player *)ent->data;
		
		inv = unlock_attack();
		inv->next = player->inventory;
		player->inventory = inv;
		entity_free(self);
	}
}

/*eol@eof*/