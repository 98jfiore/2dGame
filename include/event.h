#ifndef __EVENT_H__
#define __EVENT_H__

#include "entity.h"
#include "simple_json.h"
#include "font.h"

typedef struct
{
	Bool		_inuse;
	Vector2D	position;
	Sprite		*sprite;
	Uint32		sprite_frame;
	Vector2D	scale;
	char		*text;
	Font		*font;
	Uint32		text_size;
	Color		text_color;
}CutsceneItem;

typedef struct
{
	Rect	*triggerArea;
	char	*eventFile;
}Event;

/**
* @brief Initialize the event manager
* @param eventFile The file containing information on the triggered event
*/
void event_manager_init(char *eventFile);

/**
* @brief Free everything in the event system
*/
void event_manager_free();

/**
* @brief Update all entities in the system
*/
void event_manager_update();

/**
* @brief Draw all entities in the system
*/
void event_manager_draw();

/**
* @brief Allocate a cutscene item in the background and return a pointer to it.
* @return NULL on error or a pointer to an initialized item.
*/
CutsceneItem *cutscene_new_background();

/**
* @brief Allocate a cutscene item in the foreground and return a pointer to it.
* @return NULL on error or a pointer to an initialized item.
*/
CutsceneItem *cutscene_new_foreground();

/**
* @brief Creates a component containing text from info given
* @param text The text that will show up in the componenet
* @param fontFile The file that the font can be found on
* @param text_ptsize The pointsize of the text
* @param text_color The color of the text
* @param x The x position this component will be on
* @param y The y position this component will be on
* @param background Whether or not this item is in the background (if not, it's in the foreground).
* @return NULL on error or a pointer to a new UI Component
*/
CutsceneItem *cutscene_item_spawn_text(char *text, char *fontFile, Uint32 text_ptsize, Color text_color, int x, int y, SDL_bool background);


/**
* @brief Creates a component containing info given
* @param spriteFile The file the sprite can be found on
* @param sprite_w The width of sprites on the file
* @param sprite_h The height of sprites on the file
* @param sprite_fpl The number of sprites per like on the file
* @param sprite_num The initial sprite this sprite can be found on
* @param sprite_scale The scale this component is rendered at
* @param x The x position this component will be on
* @param y The y position this component will be on
* @param background Whether or not this item is in the background (if not, it's in the foreground).
* @return NULL on error or a pointer to a new UI Component
*/
CutsceneItem *cutscene_item_spawn_sprite(const char *spriteFile, int sprite_w, int sprite_h, int sprite_fpl, int sprite_num, int sprite_scale, int x, int y, SDL_bool background);


/**
* @brief Free a cutscene item
*/
void cutscene_item_free(CutsceneItem *self);

/**
* @brief Draw a cutscene item
*/
void cutscene_item_draw(CutsceneItem *self);

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