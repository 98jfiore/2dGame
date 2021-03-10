#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "ent_npc_automata.h"
#include "ent_npc.h"
#include "shapes.h"

Entity *automata_spawn(Vector2D position, ent_movement_flags startDir, int cycle)
{
	Entity *ent;
	Automata *automata;
	Rect *hitbox;
	Vector2D velocity;

	ent = npc_spawn(position);
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->sprite = gf2d_sprite_load_all("images/automata.png", 16, 16, 4);
	ent->frameCount = 4;
	ent->frameRate = 0.05;
	ent->scale = vector2d(2, 2);
	ent->flags = ENT_DEADLY | ENT_DESTRUCTABLE | ENT_HITTABLE;

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x - 1;
	hitbox->y = position.y - 1;
	hitbox->width = ent->sprite->frame_w * 2 - 2;
	hitbox->height = ent->sprite->frame_h * 2 - 2;
	ent->hitbox = hitbox;

	automata = malloc(sizeof(Automata));
	automata->movementFlags = startDir;
	automata->cycle = cycle;
	automata->pointInCycle = 0;

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
	ent->damage = 1;

	ent->data = automata;
	ent->think = automata_think;
	ent->free = automata_free;

	return ent;
}

void automata_think(Entity *self)
{
	Entity *collided;
	Automata *automata_self;
	Vector2D newVel;

	automata_self = (Automata *)self->data;

	if (self->hitbox != NULL)
	{
		self->hitbox->x += self->velocity.x;
		self->hitbox->y += self->velocity.y;
		//Check if npc is coliding with something, find out what it is, and do something about it.
		collided = check_collision(self);
		if (collided)
		{
			if (collided->flags & ENT_DEADLY || collided->flags & ENT_SOLID || collided->flags & ENT_TRANSITION)
			{
				self->hitbox->x -= self->velocity.x;
				self->hitbox->y -= self->velocity.y;
				newVel.x = 0;
				newVel.y = 0;

				//printf("%i\n", collided->flags);

				newVel = vector2d(0, 0);

				if (automata_self->movementFlags & MOV_NORTH)
				{
					newVel.y = 1;
					automata_self->movementFlags = MOV_SOUTH;
				}
				else if (automata_self->movementFlags & MOV_EAST)
				{
					newVel.x = -1;
					automata_self->movementFlags = MOV_WEST;
				}
				else if (automata_self->movementFlags & MOV_SOUTH)
				{
					newVel.y = -1;
					automata_self->movementFlags = MOV_NORTH;
				}
				else if (automata_self->movementFlags & MOV_WEST)
				{
					newVel.x = 1;
					automata_self->movementFlags = MOV_EAST;
				}

				self->velocity = newVel;
				//printf("%.6f, %.6f vs %.6f, %.6f\n", self->hitbox->x, self->hitbox->y, self->position.x, self->position.y);
				//printf("%.6f, %.6f vs %.6f, %.6f\n", collided->hitbox->x, collided->hitbox->y, collided->position.x, collided->position.y);
			}
		}
	}

	if (automata_self->pointInCycle >= automata_self->cycle)
	{
		newVel = vector2d(0, 0);

		if (self->velocity.x == 0 && self->velocity.y == 0)
		{
			if (automata_self->movementFlags & MOV_NORTH)
			{
				newVel.y = -1;
			}
			else if (automata_self->movementFlags & MOV_EAST)
			{
				newVel.x = 1;
			}
			else if (automata_self->movementFlags & MOV_SOUTH)
			{
				newVel.y = 1;
			}
			else if (automata_self->movementFlags & MOV_WEST)
			{
				newVel.x = -1;
			}
			self->frameRate = 0.05;
		}
		else
		{
			self->frameRate = 0;
		}

		self->velocity = newVel;

		automata_self->pointInCycle = 0;

	}

	automata_self->pointInCycle++;

}

void automata_free(Entity *self)
{
	free(self->data);
}


/*eol@eof*/