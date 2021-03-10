#include <stdlib.h>

#include "simple_logger.h"

#include "entity.h"

typedef struct
{
	Entity *entity_list;
	Uint32 max_entities;
}EntityManager;

static EntityManager entity_manager = { 0 };

void entity_manager_init(Uint32 max_entities)
{
	if (max_entities == 0)
	{
		slog("Cannot allocate 0 entities");
		return;
	}
	if (entity_manager.entity_list != NULL)
	{
		entity_manager_free();
	}

	entity_manager.entity_list = (Entity *)gfc_allocate_array(sizeof(Entity), max_entities);
	if (entity_manager.entity_list == NULL)
	{
		slog("Failed to allocate entity list");
		return;
	}
	entity_manager.max_entities = max_entities;
	atexit(entity_manager_free);
	slog("Entity system initialized");
}

void entity_manager_free()
{
	if (entity_manager.entity_list != NULL)
	{
		free(entity_manager.entity_list);
	}
	memset(&entity_manager, 0, sizeof(EntityManager));
}

Entity *delete_notplayer()
{
	int i;
	Entity *player;

	if (entity_manager.entity_list == NULL)
	{
		slog("Entity manager not initialized");
		return NULL;
	}
	player = NULL;
	for (i = 0; i < entity_manager.max_entities; ++i)
	{
		if (entity_manager.entity_list[i]._inuse == 0) continue;
		if (entity_manager.entity_list[i].flags & ENT_PLAYER) player = &entity_manager.entity_list[i];
		else
		{
			entity_manager.entity_list[i]._inuse = 0;
		}
	}
	return player;
}

void entity_update(Entity *self)
{
	if (!self) return;
	//Generic updates
	self->frame += self->frameRate;
	if (self->frame >= self->frameCount + self->baseFrame) self->frame = self->baseFrame;
	//Specific updates
	if (self->update == NULL) return;
	self->update(self);
}

void entity_manager_update_entities()
{
	int i;
	if (entity_manager.entity_list == NULL)
	{
		slog("Entity manager not initialized");
		return;
	}
	for (i = 0; i < entity_manager.max_entities; ++i)
	{
		if (entity_manager.entity_list[i]._inuse == 0) continue;
		entity_update(&entity_manager.entity_list[i]);
	}
}

void entity_draw(Entity *ent)
{
	Vector2D upperLeft;

	if (ent == NULL)
	{
		slog("CAnnot draw a NULL entity");
		return;
	}

	//If specialized draw, do it, otherwise do generic
	if (ent->draw)
	{
		ent->draw(ent);
	}
	else
	{
		if (ent->sprite == NULL)
		{
			return; //Nothing to draw
		}
		if (ent->scale.x != 0 || ent->scale.y != 0)
		{
			upperLeft = vector2d(0, 0);
			gf2d_sprite_draw(
				ent->sprite,
				ent->position,
				&ent->scale,
				&upperLeft,
				NULL,
				NULL,
				NULL,
				(Uint32)ent->frame);
		}
		else
		{
			gf2d_sprite_draw(
				ent->sprite,
				ent->position,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				(Uint32)ent->frame);
		}
	}
}

void entity_manager_draw_entities()
{
	int i;
	if (entity_manager.entity_list == NULL)
	{
		slog("Entity manager not initialized");
		return;
	}
	for (i = 0; i < entity_manager.max_entities; ++i)
	{
		if (entity_manager.entity_list[i]._inuse == 0) continue;
		entity_draw(&entity_manager.entity_list[i]);
	}
}

Entity *check_collision(Entity *self)
{
	int i;
	SDL_bool intersection;

	if (self == NULL)
	{
		slog("Cannot check colision against nonexistent entity");
		return NULL;
	}

	if (self->hitbox == NULL)
	{
		slog("Entity has no hitbox, cannot check collision against it");
		return NULL;
	}

	if (entity_manager.entity_list == NULL)
	{
		slog("Entity manager not initialized");
		return NULL;
	}

	for (i = 0; i < entity_manager.max_entities; ++i)
	{
		if (&entity_manager.entity_list[i] == self) continue;
		if (entity_manager.entity_list[i]._inuse == 0 || entity_manager.entity_list[i].hitbox == NULL) continue;
		intersection = IntersectRect(self->hitbox, entity_manager.entity_list[i].hitbox);
		if (intersection == SDL_TRUE)
		{
			return &entity_manager.entity_list[i];
		}
	}

	return NULL;
}

Entity *check_attackHit(Rect *atk, Entity *owner)
{
	int i;
	SDL_bool intersection;

	if (atk == NULL)
	{
		slog("Cannot check hits against nonexistant attack");
		return NULL;
	}

	if (entity_manager.entity_list == NULL)
	{
		slog("Entity manager not initialized");
		return NULL;
	}

	for (i = 0; i < entity_manager.max_entities; ++i)
	{
		if (&entity_manager.entity_list[i] == owner) continue;
		if (entity_manager.entity_list[i]._inuse == 0 || entity_manager.entity_list[i].hitbox == NULL) continue;
		if ((~entity_manager.entity_list[i].flags) & ENT_HITTABLE) continue;
		intersection = IntersectRect(atk, entity_manager.entity_list[i].hitbox);
		if (intersection == SDL_TRUE)
		{
			return &entity_manager.entity_list[i];
		}
	}

	return NULL;

}

void check_explosionHit(Rect *explosion, Entity *owner)
{
	int i;
	SDL_bool intersection;

	if (explosion == NULL)
	{
		slog("Cannot check hits against nonexistant attack");
		return;
	}

	if (entity_manager.entity_list == NULL)
	{
		slog("Entity manager not initialized");
		return;
	}

	for (i = 0; i < entity_manager.max_entities; ++i)
	{
		if (entity_manager.entity_list[i]._inuse == 0 || entity_manager.entity_list[i].hitbox == NULL) continue;
		intersection = IntersectRect(explosion, entity_manager.entity_list[i].hitbox);
		if (intersection == SDL_TRUE)
		{
			if (entity_manager.entity_list[i].flags & ENT_PLAYER)
			{
				player_doDamage(&entity_manager.entity_list[i], 1);
			}
			else if (entity_manager.entity_list[i].flags & ENT_DESTRUCTABLE)
			{
				entity_free(&entity_manager.entity_list[i]);
			}
		}
	}
}

Entity *entity_new()
{
	int i;
	if (entity_manager.entity_list == NULL)
	{
		slog("Entity system does not exist");
		return NULL;
	}
	for (i = 0; i < entity_manager.max_entities; ++i)
	{
		if (entity_manager.entity_list[i]._inuse) continue; //Entity space is in use
		memset(&entity_manager.entity_list[i], 0, sizeof(Entity));
		entity_manager.entity_list[i]._inuse = 1;
		entity_manager.entity_list[i].free = NULL;
		entity_manager.entity_list[i].hitbox = NULL;
		entity_manager.entity_list[i].sprite = NULL;
		return &entity_manager.entity_list[i];
	}
	slog("No free entities available");
	return NULL;
}

void entity_free(Entity *ent)
{
	if (ent == NULL)
	{
		slog("Cannot free a NULL entity");
		return;
	}
	if (ent->free != NULL)
	{
		ent->free(ent);
	}
	if (ent->sprite != NULL)
	{
		gf2d_sprite_free(ent->sprite);
		ent->sprite = NULL;
	}
	if (ent->hitbox != NULL)
	{
		free(ent->hitbox);
		ent->hitbox = NULL;
	}
	ent->_inuse = 0;
}


/*eol@eof*/