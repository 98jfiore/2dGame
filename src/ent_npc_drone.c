#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "ent_npc_drone.h"
#include "ent_npc.h"
#include "shapes.h"

Entity *drone_spawn(Vector2D position, ent_movement_flags startDir)
{
	Entity *ent;
	NPC *npc;
	Rect *hitbox;

	ent = npc_spawn(position);
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->sprite = gf2d_sprite_load_all("images/drone.png", 16, 16, 4);
	ent->frameCount = 4;
	ent->frameRate = 0.1;
	ent->scale = vector2d(2, 2);
	ent->flags = ENT_DEADLY | ENT_DESTRUCTABLE;

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x - 1;
	hitbox->y = position.y - 1;
	hitbox->width = ent->sprite->frame_w * 2 - 2;
	hitbox->height = ent->sprite->frame_h * 2 - 2;
	ent->hitbox = hitbox;

	npc = malloc(sizeof(NPC));
	npc->movementFlags = startDir;

	ent->data = npc;
	ent->think = drone_think;
	ent->free = drone_free;

	return ent;
}

void drone_think(Entity *self)
{
	Entity *collided;
	NPC *npc_self;
	Vector2D newVel;

	npc_self = (NPC *)self->data;

	newVel = vector2d(0, 0);

	if (npc_self->movementFlags & MOV_NORTH)
	{
		newVel.y = -1;
	}
	else if (npc_self->movementFlags & MOV_EAST)
	{
		newVel.x = 1;
	}
	else if (npc_self->movementFlags & MOV_SOUTH)
	{
		newVel.y = 1;
	}
	else if (npc_self->movementFlags & MOV_WEST)
	{
		newVel.x = -1;
	}


	if (self->hitbox != NULL)
	{
		self->hitbox->x += self->velocity.x;
		self->hitbox->y += self->velocity.y;
		//Check if npc is coliding with something, find out what it is, and do something about it.
		collided = check_collision(self);
		if (collided)
		{
			if (collided->flags & ENT_SOLID)
			{
				self->hitbox->x -= self->velocity.x;
				self->hitbox->y -= self->velocity.y;
				newVel.x = 0;
				newVel.y = 0;

				//printf("%i\n", collided->flags);

				if (npc_self->movementFlags & MOV_NORTH)
				{
					npc_self->movementFlags = MOV_SOUTH;
				}
				else if (npc_self->movementFlags & MOV_EAST)
				{
					npc_self->movementFlags = MOV_WEST;
				}
				else if (npc_self->movementFlags & MOV_SOUTH)
				{
					npc_self->movementFlags = MOV_NORTH;
				}
				else if (npc_self->movementFlags & MOV_WEST)
				{
					npc_self->movementFlags = MOV_EAST;
				}
				//printf("%.6f, %.6f vs %.6f, %.6f\n", self->hitbox->x, self->hitbox->y, self->position.x, self->position.y);
				//printf("%.6f, %.6f vs %.6f, %.6f\n", collided->hitbox->x, collided->hitbox->y, collided->position.x, collided->position.y);
			}
		}
	}
	self->velocity = newVel;

}

void drone_free(Entity *self)
{
	free(self->data);
}


/*eol@eof*/