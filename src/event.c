#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "event.h"
#include "player.h"

static char *playerFile = "saves/save.json";

Entity *event_trigger_spawn(Vector2D position, int width, int height, char *eventFile)
{
	Entity *ent;
	Event *evt;

	ent = entity_new();
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->flags = 0;

	evt = (Event *)malloc(sizeof(Event));
	evt->event_info = sj_load(eventFile);
	evt->triggerArea = (Rect *)malloc(sizeof(Rect));
	evt->triggerArea->x = position.x;
	evt->triggerArea->y = position.y;
	evt->triggerArea->width = width;
	evt->triggerArea->height = height;
	evt->eventFile = (char *)malloc(strlen(eventFile) * sizeof(char));
	strcpy(evt->eventFile, eventFile);
	ent->data = evt;

	ent->free = event_free;
	ent->update = event_update;

	return ent;
}

void event_free(Entity *ent)
{
	Event *evt;

	evt = (Event *)ent->data;
	if (evt == NULL) return;
	sj_free(evt->event_info);
	free(evt->triggerArea);
}

void event_update(Entity *ent)
{
	Event *evt;

	evt = (Event *)ent->data;
	if (evt == NULL) return;

	if (check_rect_collision_with_player(evt->triggerArea) == SDL_TRUE)
	{
		printf("Trigger\n");
		entity_free(ent); 
		save_playerUpgrade(playerFile, evt->eventFile);
		return;
	}
}



/*eol@eof*/