#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "ent_npc_orb.h"
#include "ent_npc.h"
#include "shapes.h"

Entity *orb_spawn(Vector2D position, ent_movement_flags startDir, int cycle)
{
	Entity *ent;
	Orb *orb;
	Rect *activeHitbox, *inactiveHitbox;
	Vector2D velocity;

	ent = npc_spawn(position);
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->sprite = gf2d_sprite_load_all("images/automata.png", 16, 16, 4);
	ent->frameCount = 1;
	ent->frameRate = 0;
	ent->scale = vector2d(2, 2);

	activeHitbox = (Rect *)malloc(sizeof(Rect));

	activeHitbox->x = position.x - 1;
	activeHitbox->y = position.y - 1;
	activeHitbox->width = ent->sprite->frame_w * 2 - 2;
	activeHitbox->height = ent->sprite->frame_h * 2 - 2;

	inactiveHitbox = (Rect *)malloc(sizeof(Rect));

	inactiveHitbox->x = position.x - 2;
	inactiveHitbox->y = position.y - 2;
	inactiveHitbox->width = ent->sprite->frame_w * 2 - 4;
	inactiveHitbox->height = ent->sprite->frame_h * 2 - 4;

	ent->flags = ENT_SOLID | ENT_DESTRUCTABLE;
	ent->hitbox = inactiveHitbox;

	orb = malloc(sizeof(Orb));
	orb->movementFlags = startDir;
	orb->cycle = cycle;
	orb->pointInCycle = 0;
	orb->activeHitbox = activeHitbox;
	orb->inactiveHitbox = inactiveHitbox;

	velocity = vector2d(0, 0);

	if (startDir & MOV_NORTH)
	{
		velocity.y = -1;
	}
	else if (startDir & MOV_EAST)
	{
		velocity.x = 1;
	}
	else if (startDir & MOV_SOUTH)
	{
		velocity.y = 1;
	}
	else if (startDir & MOV_WEST)
	{
		velocity.x = -1;
	}

	ent->velocity = velocity;

	ent->data = orb;
	ent->think = orb_think;
	ent->free = orb_free;

	return ent;
}

void orb_think(Entity *self)
{
	Entity *collided;
	Orb *orb_self;
	Vector2D newVel;

	orb_self = (Orb *)self->data;

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

				newVel = vector2d(0, 0);

				if (orb_self->movementFlags & MOV_NORTH)
				{
					newVel.y = 1;
					orb_self->movementFlags = MOV_SOUTH;
				}
				else if (orb_self->movementFlags & MOV_EAST)
				{
					newVel.x = -1;
					orb_self->movementFlags = MOV_WEST;
				}
				else if (orb_self->movementFlags & MOV_SOUTH)
				{
					newVel.y = -1;
					orb_self->movementFlags = MOV_NORTH;
				}
				else if (orb_self->movementFlags & MOV_WEST)
				{
					newVel.x = 1;
					orb_self->movementFlags = MOV_EAST;
				}

				self->velocity = newVel;
				//printf("%.6f, %.6f vs %.6f, %.6f\n", self->hitbox->x, self->hitbox->y, self->position.x, self->position.y);
				//printf("%.6f, %.6f vs %.6f, %.6f\n", collided->hitbox->x, collided->hitbox->y, collided->position.x, collided->position.y);
			}
		}
	}

	if (orb_self->pointInCycle >= orb_self->cycle)
	{
		if (self->flags & ENT_DEADLY)
		{
			self->flags = ENT_SOLID | ENT_DESTRUCTABLE;
			//self->hitbox = orb_self->inactiveHitbox;
			self->baseFrame = 0;
			self->frameCount = 1;
			self->frameRate = 0;

		}
		else
		{
			self->flags = ENT_DEADLY;
			//self->hitbox = orb_self->activeHitbox;
			self->baseFrame = 1;
			self->frameCount = 2;
			self->frameRate = 1;
		}

		orb_self->pointInCycle = 0;

	}

	orb_self->pointInCycle++;

}

void orb_free(Entity *self)
{
	Orb *orb_self;

	orb_self = (Orb *)self->data;
	free(orb_self->activeHitbox);
	free(orb_self->inactiveHitbox);

	free(self->data);
}


/*eol@eof*/