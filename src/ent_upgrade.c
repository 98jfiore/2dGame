#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "ent_upgrade.h"
#include "player.h"
#include "shapes.h"


Entity *upgrade_spawn(Vector2D position)
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

	ent->free = upgrade_free;

	return ent;
}

void upgrade_free(Entity *self)
{
	Upgrade *upgrade;

	upgrade = (Upgrade *)self->data;
	free(upgrade);
}

Entity *health_spawn(Vector2D position)
{
	Entity *ent;
	Upgrade *upgrade;
	Rect *hitbox;

	ent = upgrade_spawn(position);
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

	return NULL;
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

/*eol@eof*/