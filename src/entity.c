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

Entity *entity_new();

void entity_free(Entity *ent);

void entity_draw(Entity *ent);


/*eol@eof*/