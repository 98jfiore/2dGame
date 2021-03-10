#include "simple_logger.h"

#include "gfc_vector.h"
#include "gf2d_sprite.h"

#include "ent_environ.h"
#include "shapes.h"

Entity *environment_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale)
{
	Entity *ent;

	ent = entity_new();
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->sprite = gf2d_sprite_load_all(spriteSheet, spriteWidth, spriteHeight, fpl);
	if (ent->sprite == NULL)
	{
		slog("Sprite couldn't be loaded");
	}
	ent->frame = frameNum;
	ent->frameCount = frameNum+3;
	ent->frameRate = 0;

	ent->scale = vector2d(scale, scale);

	ent->draw = environment_draw;

	return ent;
}

Entity *wall_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale)
{
	Entity *ent;
	Rect *hitbox;

	ent = environment_spawn(position, spriteSheet, frameNum, spriteWidth, spriteHeight, fpl, scale);
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x + 3;
	hitbox->y = position.y + 3;
	hitbox->width = spriteWidth * scale - 6;
	hitbox->height = spriteHeight * scale - 6;
	ent->hitbox = hitbox;

	ent->flags = ENT_SOLID | ENT_HITTABLE;

	return ent;
}

Entity *moving_wall_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale, int speed, ent_movement_flags direction)
{
	Entity *ent;
	Rect *hitbox;
	MovingEnv *moving;

	ent = environment_spawn(position, spriteSheet, frameNum, spriteWidth, spriteHeight, fpl, scale);
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	moving = malloc(sizeof(MovingEnv));
	moving->movementFlags = direction;
	moving->speed = speed;

	ent->data = moving;

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x + 2.5;
	hitbox->y = position.y + 2.5;
	hitbox->width = spriteWidth * scale - 5;
	hitbox->height = spriteHeight * scale - 5;
	ent->hitbox = hitbox;

	ent->flags = ENT_SOLID | ENT_HITTABLE;

	ent->update = moving_wall_update;
	ent->free = moving_wall_free;

	return ent;
}

void moving_wall_free(Entity *self)
{
	free(self->data);
}

void moving_wall_update(Entity *self)
{
	Entity *collided;
	MovingEnv *moving_self;
	Vector2D newVel;

	moving_self = (MovingEnv *)self->data;

	newVel = vector2d(0, 0);

	if (moving_self->movementFlags & MOV_NORTH)
	{
		newVel.y = -1 * moving_self->speed;
	}
	else if (moving_self->movementFlags & MOV_EAST)
	{
		newVel.x = moving_self->speed;
	}
	else if (moving_self->movementFlags & MOV_SOUTH)
	{
		newVel.y = moving_self->speed;
	}
	else if (moving_self->movementFlags & MOV_WEST)
	{
		newVel.x = -1 * moving_self->speed;
	}


	if (self->hitbox != NULL)
	{
		self->hitbox->x += self->velocity.x;
		self->hitbox->y += self->velocity.y;
		//Check if npc is coliding with something, find out what it is, and do something about it.
		collided = check_collision(self);
		if (collided)
		{
			if (collided->flags & ENT_SOLID || collided->flags & ENT_PLAYER || collided->flags & ENT_TRANSITION)
			{
				self->hitbox->x -= self->velocity.x;
				self->hitbox->y -= self->velocity.y;
				newVel.x = 0;
				newVel.y = 0;


				if (moving_self->movementFlags & MOV_NORTH)
				{
					moving_self->movementFlags = MOV_SOUTH;
				}
				else if (moving_self->movementFlags & MOV_EAST)
				{
					moving_self->movementFlags = MOV_WEST;
				}
				else if (moving_self->movementFlags & MOV_SOUTH)
				{
					moving_self->movementFlags = MOV_NORTH;
				}
				else if (moving_self->movementFlags & MOV_WEST)
				{
					moving_self->movementFlags = MOV_EAST;
				}
				//printf("%.6f, %.6f vs %.6f, %.6f\n", self->hitbox->x, self->hitbox->y, self->position.x, self->position.y);
				//printf("%.6f, %.6f vs %.6f, %.6f\n", collided->hitbox->x, collided->hitbox->y, collided->position.x, collided->position.y);
			}
		}
	}
	self->velocity = newVel;
	
	vector2d_add(self->position, self->position, self->velocity);

}

Entity *pit_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale)
{
	Entity *ent;
	Rect *hitbox;

	ent = environment_spawn(position, spriteSheet, frameNum, spriteWidth, spriteHeight, fpl, scale);
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	hitbox = (Rect *)malloc(sizeof(Rect));

	/*hitbox->x = position.x + (spriteWidth * scale / 10);
	hitbox->y = position.y + (spriteHeight * scale / 10);
	hitbox->width = (spriteWidth * scale) / 20;
	hitbox->height = (spriteHeight * scale) / 20;*/
	hitbox->x = position.x + 7.5;
	hitbox->y = position.y + 7.5;
	hitbox->width = spriteWidth * scale - 15;
	hitbox->height = spriteHeight * scale - 15;
	ent->hitbox = hitbox;
	ent->damage = -1;

	ent->flags = ENT_DEADLY | ENT_NOINVIN;

	return ent;
}

Entity *spike_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale)
{
	Entity *ent;
	Rect *hitbox;

	ent = environment_spawn(position, spriteSheet, frameNum, spriteWidth, spriteHeight, fpl, scale);
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x + 5;
	hitbox->y = position.y + 5;
	hitbox->width = spriteWidth * scale - 10;
	hitbox->height = spriteHeight * scale - 10;
	ent->hitbox = hitbox;
	ent->damage = 1;

	ent->flags = ENT_DEADLY | ENT_HITTABLE;

	return ent;
}

Entity *goldDoor_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale)
{
	Entity *ent;
	Rect *hitbox;
	Door *door;

	ent = environment_spawn(position, spriteSheet, frameNum, spriteWidth, spriteHeight, fpl, scale);
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x + 2.5;
	hitbox->y = position.y + 2.5;
	hitbox->width = spriteWidth * scale - 5;
	hitbox->height = spriteHeight * scale - 5;
	ent->hitbox = hitbox;

	ent->flags = ENT_SOLID | ENT_HITTABLE | ENT_LOCKED;

	door = (Door *)malloc(sizeof(Door));
	door->keyType = malloc(strlen("goldKey") + 1);
	strcpy(door->keyType, "goldKey");
	ent->data = door;

	ent->free = free_door;

	return ent;
}

void free_door(Entity *self)
{
	Door *door;

	if (self == NULL)
	{
		slog("Can't free a null entity");
		return;
	}

	door = (Door *)self->data;
	if (door == NULL)
	{
		slog("Can't free nonexistent door");
		return;
	}

	free(door->keyType);
	free(door); 

}

void environment_draw(Entity *ent)
{
	Vector2D upperleft;

	if (ent == NULL)
	{
		slog("CAnnot draw a NULL entity");
		return;
	}

	//If specialized draw, do it, otherwise do generic
	if (ent->sprite == NULL)
	{
		return; //Nothing to draw
	}

	upperleft = vector2d(0, 0);

	gf2d_sprite_draw(
		ent->sprite,
		ent->position,
		&ent->scale,
		&upperleft,
		NULL,
		NULL,
		NULL,
		(Uint32)ent->frame);
}
/*eol@eof*/
