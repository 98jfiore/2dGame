#include "simple_logger.h"

#include "entity.h"
#include "ent_bomb.h"

Entity *bomb_spawn(Vector2D position, Entity *owner)
{
	Entity *ent;
	Bomb *bomb;

	ent = entity_new();
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->sprite = gf2d_sprite_load_all("images/bomb.png", 16, 16, 3);
	ent->frameCount = 3;
	ent->scale = owner->scale;
	ent->flags = 0;

	ent->hitbox = NULL;

	bomb = malloc(sizeof(Bomb));
	bomb->owner = owner;
	bomb->timer = 180;

	ent->frameRate = 3.0/((int)bomb->timer);

	ent->velocity = vector2d(0,0);

	ent->data = bomb;
	ent->update = bomb_update;
	ent->free = bomb_free;

	return ent;
}

void bomb_update(Entity *self)
{
	Bomb *bomb;

	if (self == NULL)
	{
		slog("Can't update NULL entity");
		return;
	}

	if (self->frame > 3)
	{
		self->frame = 3;
	}

	bomb = (Bomb *)self->data;
	if (bomb != NULL)
	{
		if (bomb->timer <= 0)
		{
			bomb_explode(self);
			return;
		}
		else
		{
			bomb->timer--;
		}
	}
}

void bomb_explode(Entity *self)
{
	Bomb *bomb;

	if (self == NULL)
	{
		slog("Can't explode NULL entity");
		return;
	}

	bomb = (Bomb *)self->data;
	explosion_spawn(self->position, bomb->owner);

	entity_free(self);
}

Entity *explosion_spawn(Vector2D position, Entity *owner)
{
	Entity *ent;
	Bomb *bomb;
	Rect *hitbox;

	ent = entity_new();
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->sprite = gf2d_sprite_load_all("images/bomb.png", 16, 16, 3);
	ent->frameCount = 12;
	ent->frame = 3;
	ent->scale = owner->scale;
	ent->flags = 0;
	ent->frameRate = 0;

	hitbox = (Rect *)malloc(sizeof(Rect));
	hitbox->x = position.x - (ent->sprite->frame_w * ent->scale.x) + 2;
	hitbox->y = position.y - (ent->sprite->frame_h * ent->scale.y) + 2;
	hitbox->width = (ent->sprite->frame_w * ent->scale.x) * 3 - 4;
	hitbox->height = (ent->sprite->frame_h * ent->scale.y) * 3 - 4;
	ent->hitbox = hitbox;

	bomb = malloc(sizeof(Bomb));
	bomb->owner = owner;
	bomb->timer = 30;

	ent->velocity = vector2d(0, 0);

	ent->data = bomb;
	ent->update = explosion_update;
	ent->draw = explosion_draw;
	ent->free = bomb_free;

	return ent;
}

void explosion_update(Entity *self)
{
	Bomb *bomb;

	if (self == NULL)
	{
		slog("Can't update NULL entity");
		return;
	}

	if (self->frame > 3)
	{
		self->frame = 3;
	}

	bomb = (Bomb *)self->data;
	if (bomb != NULL)
	{
		check_explosionHit(self->hitbox, bomb->owner);
		if (bomb->timer <= 0)
		{
			entity_free(self);
			return;
		}
		else
		{
			bomb->timer--;
		}
	}
}

void explosion_draw(Entity *self)
{
	Vector2D upperLeft, position;
	int i, j;

	if (self->sprite == NULL || self->hitbox == NULL)
	{
		return; //Nothing to draw
	}
	if (self->scale.x != 0 && self->scale.y != 0)
	{
		upperLeft = vector2d(0, 0);
		for (i = 0; i < 3; ++i)
		{
			for (j = 0; j < 3; ++j)
			{
				position = vector2d(self->hitbox->x + i * (self->sprite->frame_w * self->scale.x) - 2, self->hitbox->y + j * (self->sprite->frame_h * self->scale.y) - 2);
				gf2d_sprite_draw(
					self->sprite,
					position,
					&self->scale,
					&upperLeft,
					NULL,
					NULL,
					NULL,
					(Uint32)self->frame + i + (3*j));
			}
		}
	}
}

void bomb_free(Entity *self)
{
	free(self->data);
}

/*eol@eof*/