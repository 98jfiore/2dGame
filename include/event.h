#ifndef __EVENT_H__
#define __EVENT_H__

#include "entity.h"
#include "simple_json.h"

typedef struct
{
	Rect	*triggerArea;
	char	*eventFile;
	SJson	*event_info;
}Event;

/**
* @brief Spawn an event trigger entity
* @param position The screen position to spawn the event trigger at.
* @param width How wide the trigger area is.
* @param height How tall the trigger area is.
* @param eventFile The name of the file the event's info is saved at.
* @return NULL on error, or a pointer to a new event trigger entity.
*/
Entity *event_trigger_spawn(Vector2D position, int width, int height, char *eventFile);

/**
* @brief Free an event entity
* @param ent A pointer to the entity to free.
*/
void event_free(Entity *ent);

/**
* @brief Update an event entity
* @param ent A pointer to the entity to update.
*/
void event_update(Entity *ent);

#endif