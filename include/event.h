#ifndef __EVENT_H__
#define __EVENT_H__

#include "SDL_stdinc.h"

#include "entity.h"
#include "simple_json.h"
#include "font.h"

typedef struct
{
	Bool		_inuse;
	Vector2D	position;
	Sprite		*sprite;
	float		sprite_frame;
	float		frameRate;
	int			frameCount;
	int			baseFrame;
	Vector2D	scale;
	char		*text;
	Font		*font;
	Uint32		text_size;
	Color		text_color;
}CutsceneItem;

typedef struct
{
	Bool		_inuse;
	Vector2D	position;
	Sprite		*sprite;
	char		*tag;
	float		sprite_frame;
	float		frameRate;
	int			frameCount;
	int			baseFrame;
	Vector2D	scale;
}CutsceneActor;

typedef struct
{
	Bool		_inuse;
	char		*tag;
	Entity		*ent;
	char		*deathEventFile;
}EventSpawnedEntity;

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
* @brief Initialize the spawned entity manager
* @param numEnts The number of entities that can be spawned
*/
void spawned_manager_init(int numEnts);

/**
* @brief Free everything in the spawned entity system
*/
void spawned_manager_free();

/**
* @brief Free some tagged entity
* @param tag The tag of the entity to delete
*/
void free_spawned_ent(char *tag);

/**
* @brief Store some entity with a tag
* @param ent The enity to store
* @param tag The tag of the entity to delete
*/
void store_spawned_ent(Entity *ent, const char *tag);

/**
* @brief Store some entity with a tag
* @param ent The enity to store
* @param tag The tag of the entity to delete
* @param deathEvent The name of the file with the event to play upon death
*/
void store_spawned_ent_with_death(Entity *ent, const char *tag, const char *deathEvent);

/**
* @brief Update all entities in the system
*/
void event_manager_update();

/**
* @brief Draw all entities in the system
*/
void event_manager_draw();

/**
* @brief Is there a event manager active
* @return Boolean referencing if there is a event
*/
SDL_bool event_manager_active();

/**
* @brief Load the next event point
* @param next_point The next point to load
*/
void next_event_point(int next_point);

/**
* @brief Start a dialogue event specified in the json
* @param dialogue_event The dialogue moment to show.
*/
void start_dialogue_event_point(SJson *event_point);

/**
* @brief Start a movement event
* @param event_point The point with information on the move.
*/
void start_movement_event_point(SJson *event_point);

/**
* @brief Start a delete actor event
* @param event_point The point with information on the delete
*/
void start_deleteActor_event_point(SJson *event_point);

/**
* @brief Start a spawn event
* @param event_point The point with information on the spawn
*/
void start_spawn_event_point(SJson *event_point);

/**
* @brief Start a delete entity event
* @param event_point The point with information on the delete
*/
void start_deleteEnt_event_point(SJson *event_point);

/**
* @brief Start a stop music
*/
void start_stop_bgmusic_point();

/**
* @brief Start a start bgmusic event
* @param event_point The point with information on the music
*/
void start_start_bgmusic_point(SJson *event_point);

/**
* @brief Clear entities from the event.
*/
void event_manager_clear();

/**
* @brief Clear entities from the event.
*/
void event_manager_clear_notActor();

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
* @brief Allocate a cutscene actor return a pointer to it.
* @return NULL on error or a pointer to an initialized actor.
*/
CutsceneActor *cutscene_new_actor();

/**
* @brief Creates an actor containing info given
* @param spriteFile The file the sprite can be found on
* @param sprite_w The width of sprites on the file
* @param sprite_h The height of sprites on the file
* @param sprite_fpl The number of sprites per like on the file
* @param sprite_num The initial sprite this sprite can be found on
* @param sprite_frameRate The frame rate to advance the sprite's animation
* @param frame_count The number of frames in the sprite's animation
* @param sprite_scale The scale this component is rendered at
* @param x The x position this component will be on
* @param y The y position this component will be on
* @param tag The tag for the actor
* @return NULL on error or a pointer to a new actor
*/
CutsceneActor *cutscene_actor_spawn(const char *spriteFile, int sprite_w, int sprite_h, int sprite_fpl, int sprite_num, float sprite_frameRate, int frame_count, int sprite_scale, int x, int y, char *tag);

/**
* @brief Free a cutscene actor
*/
void cutscene_actor_free(CutsceneActor *self);

/**
* @brief Draw a cutscene actor
*/
void cutscene_actor_draw(CutsceneActor *self);

/**
* @brief update a cutscene actor
*/
void cutscene_actor_update(CutsceneActor *self);

/**
* @brief Move current actor south
*/
void move_actor_south();

/**
* @brief Move current actor north
*/
void move_actor_north();

/**
* @brief Move current actor east
*/
void move_actor_east();

/**
* @brief Move current actor west
*/
void move_actor_west();

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
* @param sprite_frameRate The frame rate to advance the sprite's animation
* @param frame_count The number of frames in the sprite's animation
* @param sprite_scale The scale this component is rendered at
* @param x The x position this component will be on
* @param y The y position this component will be on
* @param background Whether or not this item is in the background (if not, it's in the foreground).
* @return NULL on error or a pointer to a new UI Component
*/
CutsceneItem *cutscene_item_spawn_sprite(const char *spriteFile, int sprite_w, int sprite_h, int sprite_fpl, int sprite_num, float sprite_frameRate, int frame_count, int sprite_scale, int x, int y, SDL_bool background);


/**
* @brief Free a cutscene item
*/
void cutscene_item_free(CutsceneItem *self);

/**
* @brief Draw a cutscene item
*/
void cutscene_item_draw(CutsceneItem *self);

/**
* @brief update a cutscene item
*/
void cutscene_item_update(CutsceneItem *self);

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