#include "simple_logger.h"

#include "gfc_vector.h"
#include "gf2d_sprite.h"

#include "ent_environ.h"

Environ *environment_spawn(Vector2D position)
{
	Entity *ent;
	Environ *env;

	env = malloc(sizeof(Environ));

	ent = entity_new();
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->sprite = gf2d_sprite_load_all("images/perlin-grey.png", 512, 512, 1);
	ent->frameCount = 1;
	ent->frameRate = 0.1;

	env->ent = ent;
	env->flags = ENV_HITTABLE;

	ent->draw = environment_draw;

	return env;
}

void environment_draw(Entity *ent)
{
	Vector2D scale;

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
	scale = vector2d(.25, .25);
	gf2d_sprite_draw(
		ent->sprite,
		ent->position,
		&scale,
		&ent->position,
		NULL,
		NULL,
		NULL,
		(Uint32)ent->frame);
}