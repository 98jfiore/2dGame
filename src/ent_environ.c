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
	hitbox->width = spriteWidth * scale;
	hitbox->height = spriteHeight * scale;
	ent->hitbox = hitbox;

	ent->flags = ENT_SOLID;
	ent->scale = vector2d(scale, scale);

	ent->draw = environment_draw;

	return ent;
}

Entity *pit_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale)
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
	ent->frameCount = frameNum + 3;
	ent->frameRate = 0;

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

	ent->flags = ENT_DEADLY;
	ent->scale = vector2d(scale, scale);

	ent->draw = environment_draw;

	return ent;
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