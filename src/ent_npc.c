#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "ent_npc.h"
#include "shapes.h"

Entity *robot_spawn(Vector2D position)
{
	Entity *ent;
	NPC *npc;
	Rect *hitbox;

	ent = entity_new();
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->sprite = gf2d_sprite_load_all("images/robot.png", 16, 16, 4);
	ent->frameCount = 4;
	ent->frameRate = 0.05;
	ent->scale = vector2d(2, 2);
	ent->flags = ENT_DEADLY | ENT_DESTRUCTABLE;

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x;
	hitbox->y = position.y;
	hitbox->width = ent->sprite->frame_w * 2;
	hitbox->height = ent->sprite->frame_h * 2;
	ent->hitbox = hitbox;

	npc = malloc(sizeof(NPC));
	npc->movementFlags = NPC_NORTH;

	ent->data = npc;
	ent->update = npc_update;
	ent->think = robot_think;

	return ent;
}

void robot_think(Entity *self)
{
	Entity *collided;
	NPC *npc_self;
	Vector2D newVel;

	npc_self = (NPC *)self->data;

	newVel = vector2d(0, 0);

	if (npc_self->movementFlags & NPC_NORTH)
	{
		newVel.y = -5;
	}
	else if (npc_self->movementFlags & NPC_EAST)
	{
		newVel.x = 5;
	}
	else if (npc_self->movementFlags & NPC_SOUTH)
	{
		newVel.y = 5;
	}
	else if (npc_self->movementFlags & NPC_WEST)
	{
		newVel.x = -5;
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


				if (npc_self->movementFlags & NPC_NORTH)
				{
					npc_self->movementFlags = NPC_SOUTH;
				}
				else if (npc_self->movementFlags & NPC_EAST)
				{
					npc_self->movementFlags = NPC_WEST;
				}
				else if (npc_self->movementFlags & NPC_SOUTH)
				{
					npc_self->movementFlags = NPC_NORTH;
				}
				else if (npc_self->movementFlags & NPC_WEST)
				{
					npc_self->movementFlags = NPC_EAST;
				}
				//printf("%.6f, %.6f vs %.6f, %.6f\n", self->hitbox->x, self->hitbox->y, self->position.x, self->position.y);
				//printf("%.6f, %.6f vs %.6f, %.6f\n", collided->hitbox->x, collided->hitbox->y, collided->position.x, collided->position.y);
			}
		}
	}
	self->velocity = newVel;

}

Entity *npc_spawn(Vector2D position)
{
	Entity *ent;

	ent = entity_new();
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->sprite = gf2d_sprite_load_all("images/ed210_top.png", 128, 128, 16);
	ent->frameCount = 16;
	ent->frameRate = 0.1;

	ent->update = npc_update;

	return ent;
}

void npc_update(Entity *self)
{
	self->think(self);

	vector2d_add(self->position, self->position, self->velocity);
	//Move in a direction based on random
	/*rand_move = ((int)(gfc_random() * 4.0)) % 4;
	if (rand_move == 0) new_vel = vector2d(10, 0);
	else if (rand_move == 1) new_vel = vector2d(-10, 0);
	else if (rand_move == 2) new_vel = vector2d(0, 10);
	else new_vel = vector2d(0, -10);
	self->velocity = new_vel;*/

}