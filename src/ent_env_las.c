#include "simple_logger.h"

#include "ent_environ.h"
#include "ent_env_las.h"

Entity *laser_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale, int cycle, ent_movement_flags orientation)
{
	Entity *ent;
	Rect *hitbox;
	Laser *laser;

	ent = environment_spawn(position, spriteSheet, frameNum, spriteWidth, spriteHeight, fpl, scale);
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	ent->baseFrame = frameNum;
	ent->frameCount = 2;
	ent->frameRate = 0.2;

	hitbox = (Rect *)malloc(sizeof(Rect));

	if (orientation == MOV_NORTH || orientation == MOV_SOUTH)
	{
		hitbox->x = position.x + 3;
		hitbox->y = position.y;
		hitbox->width = spriteWidth * scale - 6;
		hitbox->height = spriteHeight * scale;
		ent->hitbox = hitbox;
	}
	else
	{
		hitbox->x = position.x;
		hitbox->y = position.y + 3;
		hitbox->width = spriteWidth * scale;
		hitbox->height = spriteHeight * scale - 6;
		ent->hitbox = hitbox;
	}

	ent->flags = ENT_DEADLY;

	laser = malloc(sizeof(Laser));
	laser->cycle = cycle;
	laser->pointInCycle = 0;
	laser->dirFlag = orientation;
	laser->hitbox = hitbox;

	ent->data = laser;
	ent->damage = 1;

	ent->update = laser_update;
	ent->draw = laser_draw;

	return ent;

}

Entity *laserBase_spawn(Vector2D position, char *spriteSheet, int frameNum, int laserFrameNum, int spriteWidth, int spriteHeight, int fpl, int scale, int cycle, int range, ent_movement_flags orientation)
{
	Entity *ent;
	Rect *hitbox;
	Vector2D laserPos;
	int i;

	if (orientation == MOV_EAST)
	{
		ent = environment_spawn(position, spriteSheet, frameNum, spriteWidth, spriteHeight, fpl, scale);

		if (ent == NULL)
		{
			slog("Failed to create entity");
			return NULL;
		}

		for (i = 1; i <= range; i++)
		{
			laserPos = vector2d(position.x + i * spriteWidth * scale, position.y);
			laser_spawn(laserPos, spriteSheet, laserFrameNum, spriteWidth, spriteHeight, fpl, scale, cycle, orientation);
		}
	}
	else if (orientation == MOV_WEST)
	{
		ent = environment_spawn(position, spriteSheet, frameNum + 1, spriteWidth, spriteHeight, fpl, scale);

		if (ent == NULL)
		{
			slog("Failed to create entity");
			return NULL;
		}

		for (i = 1; i <= range; i++)
		{
			laserPos = vector2d(position.x - i * spriteWidth * scale, position.y);
			laser_spawn(laserPos, spriteSheet, laserFrameNum, spriteWidth, spriteHeight, fpl, scale, cycle, orientation);
		}
	}
	else if (orientation == MOV_NORTH)
	{
		ent = environment_spawn(position, spriteSheet, frameNum + 2, spriteWidth, spriteHeight, fpl, scale);

		if (ent == NULL)
		{
			slog("Failed to create entity");
			return NULL;
		}

		for (i = 1; i <= range; i++)
		{
			laserPos = vector2d(position.x, position.y - i * spriteHeight * scale);
			laser_spawn(laserPos, spriteSheet, laserFrameNum, spriteWidth, spriteHeight, fpl, scale, cycle, orientation);
		}
	}
	else
	{
		ent = environment_spawn(position, spriteSheet, frameNum + 3, spriteWidth, spriteHeight, fpl, scale);

		if (ent == NULL)
		{
			slog("Failed to create entity");
			return NULL;
		}

		for (i = 1; i <= range; i++)
		{
			laserPos = vector2d(position.x, position.y + i * spriteHeight * scale);
			laser_spawn(laserPos, spriteSheet, laserFrameNum, spriteWidth, spriteHeight, fpl, scale, cycle, orientation);
		}
	}

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x + 2.5;
	hitbox->y = position.y + 2.5;
	hitbox->width = spriteWidth * scale - 5;
	hitbox->height = spriteHeight * scale - 5;
	ent->hitbox = hitbox;

	ent->flags = ENT_SOLID | ENT_HITTABLE;

	return ent;

}

void laser_draw(Entity * self)
{
	Vector2D upperLeft;
	Vector3D rotation;
	Laser *laser;

	laser = (Laser *)self->data;

	if (self->sprite == NULL || self->hitbox == NULL)
	{
		return; //Nothing to draw
	}
	if (self->scale.x != 0 || self->scale.y != 0)
	{
		upperLeft = vector2d(0, 0);
		if (laser->dirFlag == MOV_EAST || laser->dirFlag == MOV_WEST)
		{
			//rotation = vector3d(self->position.x + ((self->sprite->frame_w * self->scale.x) / 2), self->position.y + ((self->sprite->frame_h * self->scale.y) / 2), 90);
			rotation = vector3d((self->sprite->frame_w) / 2, (self->sprite->frame_h) / 2, 90);
			gf2d_sprite_draw(
				self->sprite,
				self->position,
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
				self->position,
				&self->scale,
				&upperLeft,
				NULL,
				NULL,
				NULL,
				(Uint32)self->frame);
		}
	}
	else
	{
		gf2d_sprite_draw(
			self->sprite,
			self->position,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			(Uint32)self->frame);
	}
}

void laser_update(Entity *self)
{
	Laser *laser;

	laser = (Laser *)self->data;
	laser->pointInCycle++;
	if (laser->pointInCycle > laser->cycle)
	{
		laser->pointInCycle = 0;
		if (self->hitbox == NULL)
		{
			self->hitbox = laser->hitbox;
		}
		else
		{
			self->hitbox = NULL;
		}
	}
}

void laser_free(Entity *self)
{
	Laser *laser;

	laser = (Laser *)self->data;
	free(laser->hitbox);
	free(self->data);
}

/*eol@eof*/
