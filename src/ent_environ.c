#include "simple_logger.h"

#include "gfc_vector.h"
#include "gf2d_sprite.h"

#include "ent_environ.h"
#include "shapes.h"

Entity *environment_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale)
{
	Entity *ent;
	Rect *hitbox;

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

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x;
	hitbox->y = position.y;
	hitbox->width = ent->sprite->frame_w;
	hitbox->height = ent->sprite->frame_w;
	ent->hitbox = hitbox;

	ent->flags = ENT_SOLID;
	ent->scale = vector2d(scale, scale);

	ent->draw = environment_draw;

	return ent;
}

void environment_draw(Entity *ent)
{
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
	gf2d_sprite_draw(
		ent->sprite,
		ent->position,
		&ent->scale,
		&ent->scale,
		NULL,
		NULL,
		NULL,
		(Uint32)ent->frame);
}