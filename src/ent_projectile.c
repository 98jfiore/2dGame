#include "simple_logger.h"

#include "entity.h"
#include "ent_projectile.h"

Entity *projectile_spawn(Vector2D position, ent_movement_flags startDir, Entity *owner)
{
	Entity *ent;
	Projectile *proj;
	Rect *hitbox;
	Vector2D vel;

	ent = entity_new();
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->sprite = gf2d_sprite_load_all("images/swordbeam.png", 16, 16, 4);
	ent->frameCount = 4;
	ent->frameRate = 0.05;
	ent->scale = vector2d(2, 2);
	ent->flags = 0;

	hitbox = (Rect *)malloc(sizeof(Rect));

	if (startDir == MOV_NORTH || startDir == MOV_SOUTH)
	{
		hitbox->x = position.x + 3;
		hitbox->y = position.y + 1;
		hitbox->width = ent->sprite->frame_w * 2 - 6;
		hitbox->height = ent->sprite->frame_h * 2 - 2;
	}
	else
	{
		hitbox->x = position.x + 1;
		hitbox->y = position.y + 3;
		hitbox->width = ent->sprite->frame_w * 2 - 2;
		hitbox->height = ent->sprite->frame_h * 2 - 6;
	}

	ent->hitbox = hitbox;
	ent->damage = 1;

	proj = malloc(sizeof(Projectile));
	proj->owner = owner;

	if (startDir == MOV_EAST)
	{
		proj->rotate = vector3d((ent->sprite->frame_w) / 2, (ent->sprite->frame_h) / 2, 90);
		vel = vector2d(10, 0);
	}
	else if (startDir == MOV_SOUTH)
	{
		proj->rotate = vector3d((ent->sprite->frame_w) / 2, (ent->sprite->frame_h) / 2, 180);
		vel = vector2d(0, 10);
	}
	else if (startDir == MOV_WEST)
	{
		proj->rotate = vector3d((ent->sprite->frame_w) / 2, (ent->sprite->frame_h) / 2, 270);
		vel = vector2d(-10, 0);
	}
	else
	{
		proj->rotate = vector3d((ent->sprite->frame_w) / 2, (ent->sprite->frame_h) / 2, 0);
		vel = vector2d(0, -10);
	}

	ent->velocity = vel;

	proj->life = 20;

	ent->data = proj;
	ent->update = projectile_update;
	ent->free = projectile_free;
	ent->draw = projectile_draw;

	return ent;
}

void projectile_update(Entity *self)
{
	Projectile *proj;
	Entity *collided;

	if (self == NULL)
	{
		slog("Can't update NULL entity");
		return;
	}

	if (self->hitbox != NULL)
	{
		self->hitbox->x += self->velocity.x;
		self->hitbox->y += self->velocity.y;

		collided = check_collision(self);
		if (collided)
		{
			if (collided->flags & ENT_DESTRUCTABLE)
			{
				entity_free(collided);
				entity_free(self);
			}
			else if (collided->flags & ENT_SOLID || collided->flags & ENT_TRANSITION)
			{
				entity_free(self);
			}
			else if (collided->flags & ENT_SOLID || collided->flags & ENT_TRANSITION)
			{
				entity_free(self);
			}
			else if (collided->flags & ENT_HASHEALTH)
			{
				hasHealth_doDamage(collided, 1);

				entity_free(self);
			}
		}
	}

	vector2d_add(self->position, self->position, self->velocity);

	proj = (Projectile *)self->data;
	if (proj != NULL)
	{
		if (proj->life <= 0)
		{
			entity_free(self);
		}
		else
		{
			proj->life--;
		}
	}
}


void projectile_free(Entity *self)
{
	free(self->data);
}

void projectile_draw(Entity *self)
{
	Vector2D upperLeft;
	Projectile *proj;

	proj = (Projectile *)self->data;

	if (self->sprite == NULL || self->hitbox == NULL)
	{
		return; //Nothing to draw
	}
	if (self->scale.x != 0 || self->scale.y != 0)
	{
		upperLeft = vector2d(0, 0);
		gf2d_sprite_draw(
			self->sprite,
			self->position,
			&self->scale,
			&upperLeft,
			&proj->rotate,
			NULL,
			NULL,
			(Uint32)self->frame);
	}
	else
	{
		gf2d_sprite_draw(
			self->sprite,
			self->position,
			NULL,
			NULL,
			&proj->rotate,
			NULL,
			NULL,
			(Uint32)self->frame);
	}
}

/*eol@eof*/