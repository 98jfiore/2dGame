#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "event.h"
#include "game_state.h"
#include "player.h"

static char *playerFile = "saves/save.json";

typedef struct
{
	CutsceneItem *background_items;
	CutsceneItem *foreground_items;
	SJson		*event_actions;
	Uint32		event_point;
	Uint32		event_length;
	Uint32		action_wait;
	Entity		*actor;
}EventManager;

static EventManager event_manager = { 0 };

void event_manager_init(char *eventFile)
{
	if (event_manager.background_items != NULL || event_manager.foreground_items != NULL)
	{
		event_manager_free();
	}

	event_manager.background_items = (CutsceneItem *)gfc_allocate_array(sizeof(CutsceneItem), 5);
	if (event_manager.background_items == NULL)
	{
		slog("Failed to allocate event component list");
		return;
	}
	event_manager.foreground_items = (CutsceneItem *)gfc_allocate_array(sizeof(CutsceneItem), 5);
	if (event_manager.foreground_items == NULL)
	{
		slog("Failed to allocate event component list");
		return;
	}
	event_manager.action_wait = 40;
	event_manager.event_actions = sj_load(eventFile);
	event_manager.event_point = 0;
	event_manager.event_length = sj_array_get_count(event_manager.event_actions);
	event_manager.actor = NULL;
	slog("Event initialized");
}

void event_manager_free()
{
	if (event_manager.background_items != NULL)
	{
		free(event_manager.background_items);
	}
	if (event_manager.foreground_items != NULL)
	{
		free(event_manager.foreground_items);
	}
	memset(&event_manager, 0, sizeof(EventManager));
}

void event_manager_update()
{
	//int i;
	int clickX, clickY;
	Uint8 *state;

	/*if (event_manager.background_items == NULL)
	{
		slog("Event manager not initialized");
		return;
	}
	for (i = 0; i < 5; ++i)
	{
		if (event_manager.background_items[i]._inuse == 0) continue;
		cutscene_item_update(&event_manager.background_items[i]);
	}

	if (event_manager.foreground_items == NULL)
	{
		slog("Event manager not initialized");
		return;
	}
	for (i = 0; i < 5; ++i)
	{
		if (event_manager.foreground_items[i]._inuse == 0) continue;
		cutscene_item_update(&event_manager.foreground_items[i]);
	}*/


	//IF USER MOVE FORWARD, MOVE FORWARD

	if (event_manager.foreground_items == NULL || event_manager.background_items == NULL)
	{
		//slog("UI manager not initialized");
		return;
	}

	state = SDL_GetKeyboardState(NULL);
	if (event_manager.action_wait > 0)
	{
		event_manager.action_wait--;
	}
	else if (state[SDL_SCANCODE_RETURN] || state[SDL_SCANCODE_KP_ENTER] || SDL_GetMouseState(&clickX, &clickY) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		event_manager.action_wait = 30;
		event_manager.event_point++;
		event_manager_free();
		resume_game();
	}
}

void event_manager_draw()
{
	int i;

	if (event_manager.background_items == NULL)
	{
		//slog("Event manager not initialized");
		return;
	}
	for (i = 0; i < 5; ++i)
	{
		if (event_manager.background_items[i]._inuse == 0) continue;
		cutscene_item_draw(&event_manager.background_items[i]);
	}

	if (event_manager.foreground_items == NULL)
	{
		slog("Event manager not initialized");
		return;
	}
	for (i = 0; i < 5; ++i)
	{
		if (event_manager.foreground_items[i]._inuse == 0) continue;
		cutscene_item_draw(&event_manager.foreground_items[i]);
	}
}

CutsceneItem *cutscene_new_background()
{
	int i;
	if (event_manager.background_items == NULL)
	{
		slog("Event system does not exist");
		return NULL;
	}
	for (i = 0; i < 5; ++i)
	{
		if (event_manager.background_items[i]._inuse) continue; //Entity space is in use
		memset(&event_manager.background_items[i], 0, sizeof(Entity));
		event_manager.background_items[i]._inuse = 1;
		event_manager.background_items[i].text = NULL;
		event_manager.background_items[i].font = NULL;
		event_manager.background_items[i].sprite = NULL;
		return &event_manager.background_items[i];
	}
	slog("No free entities available");
	return NULL;
}

CutsceneItem *cutscene_new_foreground()
{
	int i;
	if (event_manager.foreground_items == NULL)
	{
		slog("Event system does not exist");
		return NULL;
	}
	for (i = 0; i < 5; ++i)
	{
		if (event_manager.foreground_items[i]._inuse) continue; //Entity space is in use
		memset(&event_manager.foreground_items[i], 0, sizeof(Entity));
		event_manager.foreground_items[i]._inuse = 1;
		event_manager.foreground_items[i].text = NULL;
		event_manager.foreground_items[i].font = NULL;
		event_manager.foreground_items[i].sprite = NULL;
		return &event_manager.foreground_items[i];
	}
	slog("No free entities available");
	return NULL;
}

CutsceneItem *cutscene_item_spawn_text(char *text, char *fontFile, Uint32 text_ptsize, Color text_color, int x, int y, SDL_bool background)
{
	CutsceneItem *item;

	if (background == SDL_TRUE) item = cutscene_new_background();
	else item = cutscene_new_foreground();

	if (item == NULL)
	{
		slog("No more cutscene items");
		return NULL;
	}

	item->font = font_load(fontFile, text_ptsize);
	item->text_color = text_color;
	item->text = malloc((strlen(text) + 1) * sizeof(char));
	strcpy(item->text, text);

	item->position = vector2d(x, y);
	return item;
}

CutsceneItem *cutscene_item_spawn_sprite(const char *spriteFile, int sprite_w, int sprite_h, int sprite_fpl, int sprite_num, int sprite_scale, int x, int y, SDL_bool background)
{
	CutsceneItem *item;

	if (background == SDL_TRUE) item = cutscene_new_background();
	else item = cutscene_new_foreground();

	if (item == NULL)
	{
		slog("No more cutscene items");
		return NULL;
	}

	item->position = vector2d(x, y);

	item->sprite = gf2d_sprite_load_all(spriteFile, sprite_w, sprite_h, sprite_fpl);
	if (item->sprite == NULL)
	{
		slog("Event sprite couldn't load");
		free(item);
		return NULL;
	}
	item->sprite_frame = sprite_num;
	item->scale = vector2d(sprite_scale, sprite_scale);

	item->text = NULL;

	return item;
}

void cutscene_item_free(CutsceneItem *self)
{
	if (self == NULL)
	{
		slog("Cannot free a NULL component");
		return;
	}
	if (self->sprite != NULL)
	{
		gf2d_sprite_free(self->sprite);
		self->sprite = NULL;
	}
	if (self->text != NULL)
	{
		free(self->text);
		self->text = NULL;
	}
	self->_inuse = 0;
}

void cutscene_item_draw(CutsceneItem *self)
{
	Vector2D upperLeft;

	if (self->sprite != NULL)
	{
		if (self->scale.x != 0 || self->scale.y != 0)
		{
			upperLeft = vector2d(0, 0);
			gf2d_sprite_draw(
				self->sprite,
				self->position,
				&self->scale,
				&upperLeft,
				NULL,
				NULL,
				NULL,
				self->sprite_frame);
		}
		else
		{
			gf2d_sprite_draw(
				self->sprite,
				self->position,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				self->sprite_frame);
		}
	}
	if (self->text != NULL && self->font != NULL)
	{
		font_render(self->font, self->text, self->text_color, self->position);
	}
}

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
	free(evt->triggerArea);
}

void event_update(Entity *ent)
{
	Event *evt;

	evt = (Event *)ent->data;
	if (evt == NULL) return;

	if (check_rect_collision_with_player(evt->triggerArea) == SDL_TRUE)
	{
		event_manager_init(evt->eventFile);
		entity_free(ent); 
		save_playerUpgrade(playerFile, evt->eventFile);
		start_cutscene_game();
		return;
	}
}



/*eol@eof*/