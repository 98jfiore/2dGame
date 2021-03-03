#include "simple_logger.h"

#include "player.h"
#include "attack.h"

Attack *start_attack(Entity *owner, ent_movement_flags direction)
{
	Attack *atk;
	Rect *hitbox;

	if (owner == NULL)
	{
		slog("Cannot have an attack with no owner");
		return NULL;
	}

	atk = (Attack *)malloc(sizeof(Attack));
	if (atk == NULL)
	{
		slog("Could not allocate attack");
		return NULL;
	}

	atk->owner = owner;
	atk->direction = direction;

	vector2d_copy(atk->scale, owner->scale);
	if (atk->scale.x == 0) atk->scale.x = 1;
	if (atk->scale.y == 0) atk->scale.y = 1;

	atk->sprite = gf2d_sprite_load_all("images/sword_swing.png", 16, 16, 4);
	if (atk->sprite == NULL)
	{
		slog("FAILED to load attack sprite");
		return NULL;
	}
	atk->frame = 0;
	atk->frameRate = 0.25;
	atk->lastFrame = 4;

	atk->damage = 1;

	if (owner->sprite == NULL)
	{
		slog("Owner doesn't have a sprite");
		free(atk);
		return NULL;
	}
	hitbox = (Rect *)malloc(sizeof(Rect));

	if (direction == MOV_NORTH)
	{
		hitbox->x = owner->position.x + 1;
		hitbox->y = owner->position.y - (owner->sprite->frame_h * owner->scale.y) + 1;
		hitbox->width = atk->sprite->frame_w * atk->scale.x - 2;
		hitbox->height = atk->sprite->frame_h * atk->scale.y - 2;

	}
	else if (direction == MOV_SOUTH)
	{
		hitbox->x = owner->position.x + 1;
		hitbox->y = owner->position.y + (owner->sprite->frame_h * owner->scale.y) + 1;
		hitbox->width = atk->sprite->frame_w * atk->scale.x - 2;
		hitbox->height = atk->sprite->frame_h * atk->scale.y - 2;

	}
	else if (direction == MOV_EAST)
	{
		hitbox->x = owner->position.x + (owner->sprite->frame_w * owner->scale.x) + 1;
		hitbox->y = owner->position.y + 1;
		hitbox->width = atk->sprite->frame_w * atk->scale.x - 2;
		hitbox->height = atk->sprite->frame_h * atk->scale.y - 2;

	}
	else
	{
		hitbox->x = owner->position.x - (owner->sprite->frame_w * owner->scale.x) + 1;
		hitbox->y = owner->position.y + 1;
		hitbox->width = atk->sprite->frame_w * atk->scale.x - 2;
		hitbox->height = atk->sprite->frame_h * atk->scale.y - 2;
	}

	atk->hitbox = hitbox;

	atk->update = NULL;
	atk->draw = NULL;
	atk->free = NULL;

	return atk;
}

void attack_update(Attack *self)
{
	Entity *hit;

	if (!self) return;
	//Generic updates
	self->frame += self->frameRate;
	if (self->frame >= self->lastFrame)
	{
		attack_free(self);
		return;
	}
	//Specific updates
	if (self->direction == MOV_NORTH)
	{
		self->hitbox->x = self->owner->position.x + 1;
		self->hitbox->y = self->owner->position.y - (self->owner->sprite->frame_h * self->owner->scale.y) + 1;
	}
	else if (self->direction == MOV_SOUTH)
	{
		self->hitbox->x = self->owner->position.x + 1;
		self->hitbox->y = self->owner->position.y + (self->owner->sprite->frame_h * self->owner->scale.y) + 1;
	}
	else if (self->direction == MOV_EAST)
	{
		self->hitbox->x = self->owner->position.x + (self->owner->sprite->frame_w * self->owner->scale.x) + 1;
		self->hitbox->y = self->owner->position.y + 1;
	}
	else
	{
		self->hitbox->x = self->owner->position.x - (self->owner->sprite->frame_w * self->owner->scale.x) + 1;
		self->hitbox->y = self->owner->position.y + 1;
	}

	hit = check_attackHit(self->hitbox, self->owner);
	if (hit != NULL)
	{
		if (hit->flags & ENT_DESTRUCTABLE)
		{
			entity_free(hit);
		}
	}


	if (self->update == NULL) return;
	self->update(self);
}


void attack_draw(Attack *self)
{
	Vector2D upperLeft, position;
	Vector3D rotation;

	if (self->sprite == NULL)
	{
		return; //Nothing to draw
	}
	if (self->scale.x != 0 || self->scale.y != 0)
	{
		upperLeft = vector2d(0, 0);
		position = vector2d(self->hitbox->x - 1, self->hitbox->y - 1);
		if (self->direction == MOV_EAST)
		{
			//rotation = vector3d(self->position.x + ((self->sprite->frame_w * self->scale.x) / 2), self->position.y + ((self->sprite->frame_h * self->scale.y) / 2), 90);
			rotation = vector3d((self->sprite->frame_w) / 2, (self->sprite->frame_h) / 2, 90);
			gf2d_sprite_draw(
				self->sprite,
				position,
				&self->scale,
				&upperLeft,
				&rotation,
				NULL,
				NULL,
				(Uint32)self->frame);
		}
		else if (self->direction == MOV_SOUTH)
		{
			//rotation = vector3d(self->position.x + ((self->sprite->frame_w * self->scale.x) / 2), self->position.y + ((self->sprite->frame_h * self->scale.y) / 2), 90);
			rotation = vector3d((self->sprite->frame_w) / 2, (self->sprite->frame_h) / 2, 180);
			gf2d_sprite_draw(
				self->sprite,
				position,
				&self->scale,
				&upperLeft,
				&rotation,
				NULL,
				NULL,
				(Uint32)self->frame);
		}
		else if (self->direction == MOV_WEST)
		{
			//rotation = vector3d(self->position.x + ((self->sprite->frame_w * self->scale.x) / 2), self->position.y + ((self->sprite->frame_h * self->scale.y) / 2), 90);
			rotation = vector3d((self->sprite->frame_w) / 2, (self->sprite->frame_h) / 2, 270);
			gf2d_sprite_draw(
				self->sprite,
				position,
				&self->scale,
				&upperLeft,
				&rotation,
				NULL,
				NULL,
				(Uint32)self->frame);
		}
		else
		{
			gf2d_sprite_draw(
				self->sprite,
				position,
				&self->scale,
				&upperLeft,
				NULL,
				NULL,
				NULL,
				(Uint32)self->frame);
		}
	}
	if (self->draw == NULL) return;
	self->draw(self);
}

void attack_free(Attack *self)
{
	Player *player_owner;

	if (self == NULL)
	{
		slog("Cannot free a NULL attack");
		return;
	}

	player_owner = (Player *)self->owner->data;
	if (player_owner != NULL)
	{
		player_owner->attack = NULL;
	}
	gf2d_sprite_free(self->sprite);
	self->sprite = NULL;
	free(self->hitbox);
	self->hitbox = NULL;
	if (self->free == NULL) return;
	self->free(self);
}

/*eol@eof*/
