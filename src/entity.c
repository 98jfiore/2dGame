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
	gf2d_sprite_free(ent->sprite);
	ent->sprite = NULL;
	ent->_inuse = 0;
}

void entity_draw(Entity *ent)
{
	if (ent == NULL)
	{
		slog("CAnnot draw a NULL entity");
		return;
	}
	if (ent->sprite == NULL)
	{
		return; //Nothing to draw
	}
	gf2d_sprite_draw(
		ent->sprite,
		ent->position,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		(Uint32) ent->frame);
}


/*eol@eof*/