#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "ent_npc_sweeper.h"
#include "ent_npc.h"
#include "shapes.h"

Entity *sweeper_spawn(Vector2D position, ent_movement_flags startDir, int cycle)
{
	Entity *ent;
	Sweeper *sweeper;
	Rect *hitbox;

	ent = npc_spawn(position);
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->sprite = gf2d_sprite_load_all("images/sweeper.png", 16, 16, 4);
	ent->frameCount = 4;
	ent->frameRate = 0.1;
	ent->scale = vector2d(2, 2);
	ent->flags = ENT_DEADLY | ENT_DESTRUCTABLE | ENT_HITTABLE;

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x - 0.5;
	hitbox->y = position.y - 0.5;
	hitbox->width = ent->sprite->frame_w * 2 - 1;
	hitbox->height = ent->sprite->frame_h * 2 - 1;
	ent->hitbox = hitbox;

	sweeper = malloc(sizeof(Sweeper));
	sweeper->movementFlags = startDir;
	sweeper->cycle = cycle;
	sweeper->pointInCycle = 0;

	ent->data = sweeper;
	ent->damage = 1;

	ent->think = sweeper_think;
	ent->free = sweeper_free;

	return ent;
}

void sweeper_think(Entity *self)
{
	Entity *collided;
	Sweeper *sweeper_self;
	Vector2D newVel;
	int randDir;

	sweeper_self = (Sweeper *)self->data;

	newVel = vector2d(0, 0);

	if (sweeper_self->movementFlags & MOV_NORTH)
	{
		newVel.y = -2;
	}
	else if (sweeper_self->movementFlags & MOV_EAST)
	{
		newVel.x = 2;
	}
	else if (sweeper_self->movementFlags & MOV_SOUTH)
	{
		newVel.y = 2;
	}
	else if (sweeper_self->movementFlags & MOV_WEST)
	{
		newVel.x = -2;
	}


	if (self->hitbox != NULL)
	{
		self->hitbox->x += self->velocity.x;
		self->hitbox->y += self->velocity.y;
		//Check if npc is coliding with something, find out what it is, and do something about it.
		collided = check_collision(self);
		if (collided)
		{
			if (collided->flags & ENT_DEADLY || collided->flags & ENT_SOLID)
			{
				self->hitbox->x -= self->velocity.x;
				self->hitbox->y -= self->velocity.y;
				newVel.x = 0;
				newVel.y = 0;

				//printf("%i\n", collided->flags);

				if (sweeper_self->movementFlags & MOV_NORTH)
				{
					sweeper_self->movementFlags = MOV_SOUTH;
				}
				else if (sweeper_self->movementFlags & MOV_EAST)
				{
					sweeper_self->movementFlags = MOV_WEST;
				}
				else if (sweeper_self->movementFlags & MOV_SOUTH)
				{
					sweeper_self->movementFlags = MOV_NORTH;
				}
				else if (sweeper_self->movementFlags & MOV_WEST)
				{
					sweeper_self->movementFlags = MOV_EAST;
				}
				//printf("%.6f, %.6f vs %.6f, %.6f\n", self->hitbox->x, self->hitbox->y, self->position.x, self->position.y);
				//printf("%.6f, %.6f vs %.6f, %.6f\n", collided->hitbox->x, collided->hitbox->y, collided->position.x, collided->position.y);
			}
		}
	}

	if (sweeper_self->pointInCycle >= sweeper_self->cycle)
	{
		randDir = ((int)(gfc_random() * 4.0)) % 4;
		if (randDir == 0) sweeper_self->movementFlags = MOV_NORTH;
		else if (randDir == 1) sweeper_self->movementFlags = MOV_SOUTH;
		else if (randDir == 2) sweeper_self->movementFlags = MOV_EAST;
		else sweeper_self->movementFlags = MOV_WEST;

		sweeper_self->pointInCycle = 0;

	}

	sweeper_self->pointInCycle++;

	self->velocity = newVel;

}

void sweeper_free(Entity *self)
{
	free(self->data);
}


/*eol@eof*/