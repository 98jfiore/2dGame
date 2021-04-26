#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"
#include "gfc_audio.h"

#include "event.h"
#include "game_state.h"
#include "player.h"
#include "bosses.h"
#include "ent_environ.h"

static char *playerFile = "saves/save.json";

typedef struct
{
	CutsceneActor *actors;
	CutsceneItem *background_items;
	CutsceneItem *foreground_items;
	SJson		*event_actions;
	SJson		*current_event;
	Uint32		event_point;
	Uint32		event_length;
	Uint32		action_wait;
	Uint32		num_actors;
	Uint32		cur_actor;
	float		action_speed;
	void		(*curr_action)();
}EventManager;

typedef struct
{
	EventSpawnedEntity *spawnedEnts;
	Uint32				max_spawns;
}SpawnedEntManager;

static EventManager event_manager = { 0 };
static SpawnedEntManager spawned_manager = { 0 };
static SDL_bool advance_scene = SDL_FALSE;

void event_manager_init(char *eventFile)
{
	SJson *basejs, *actorsjs, *actorlistjs, *jsactor;
	int actNum, initialActNum, i;
	int x, y, sprite_w, sprite_h, sprite_fpl, sprite_num, frame_count, sprite_scale;
	float sprite_frameRate;
	char *sprite_file, *tag;

	if (event_manager.background_items != NULL || event_manager.foreground_items != NULL)
	{
		event_manager_free();
	}

	event_manager.background_items = (CutsceneItem *)gfc_allocate_array(sizeof(CutsceneItem), 5);
	if (event_manager.background_items == NULL)
	{
		slog("Failed to allocate event component list");
		event_manager_free();
		resume_game();
		return;
	}
	event_manager.foreground_items = (CutsceneItem *)gfc_allocate_array(sizeof(CutsceneItem), 5);
	if (event_manager.foreground_items == NULL)
	{
		slog("Failed to allocate event component list");
		event_manager_free();
		resume_game();
		return;
	}
	event_manager.action_wait = 40;
	basejs = sj_load(eventFile);

	event_manager.event_actions = sj_object_get_value(basejs, "events");
	event_manager.event_point = 0;
	event_manager.event_length = sj_array_get_count(event_manager.event_actions);
	//slog("%i", event_manager.event_length);
	//slog(eventFile);

	actorsjs = sj_object_get_value(basejs, "actors");
	if (actorsjs != NULL)
	{
		sj_get_integer_value(sj_object_get_value(actorsjs, "num_actors"), &actNum);
		sj_get_integer_value(sj_object_get_value(actorsjs, "num_initial_actors"), &initialActNum);

		event_manager.actors = (CutsceneActor *)gfc_allocate_array(sizeof(CutsceneActor), actNum);
		event_manager.num_actors = actNum;

		if (initialActNum > 0)
		{
			actorlistjs = sj_object_get_value(actorsjs, "init_actors");
			for (i = 0; i < initialActNum; ++i)
			{
				jsactor = sj_array_get_nth(actorlistjs, i);
				if (jsactor == NULL) continue;
				sj_get_integer_value(sj_object_get_value(jsactor, "x"), &x);
				sj_get_integer_value(sj_object_get_value(jsactor, "y"), &y);
				sprite_file = sj_get_string_value(sj_object_get_value(jsactor, "sprite_file"));
				tag = sj_get_string_value(sj_object_get_value(jsactor, "tag"));
				sj_get_integer_value(sj_object_get_value(jsactor, "sprite_w"), &sprite_w);
				sj_get_integer_value(sj_object_get_value(jsactor, "sprite_h"), &sprite_h);
				sj_get_integer_value(sj_object_get_value(jsactor, "sprite_fpl"), &sprite_fpl);
				sj_get_integer_value(sj_object_get_value(jsactor, "sprite_num"), &sprite_num);
				sj_get_float_value(sj_object_get_value(jsactor, "sprite_frameRate"), &sprite_frameRate);
				sj_get_integer_value(sj_object_get_value(jsactor, "sprite_frame_count"), &frame_count);
				sj_get_integer_value(sj_object_get_value(jsactor, "sprite_scale"), &sprite_scale);

				cutscene_actor_spawn(sprite_file, sprite_w, sprite_h, sprite_fpl, sprite_num, sprite_frameRate, frame_count, sprite_scale, x, y, tag);
				//cutscene_item_spawn_sprite(sprite_file, sprite_w, sprite_h, sprite_fpl, sprite_num, sprite_frameRate, frame_count, sprite_scale, x * 32, y * 32, SDL_TRUE);

			}
		}
	}
	else
	{
		event_manager.actors = NULL;
		event_manager.num_actors = 0;
	}

	event_manager.current_event = NULL;
	event_manager.curr_action = NULL;
	event_manager.cur_actor = 0;
	slog("Event initialized");
	advance_scene = SDL_FALSE;

	next_event_point(0);

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
	if (event_manager.actors != NULL)
	{
		free(event_manager.actors);
	}
	memset(&event_manager, 0, sizeof(EventManager));
}


void spawned_manager_init(int numEnts)
{
	if (spawned_manager.spawnedEnts != NULL)
	{
		spawned_manager_free();
	}

	spawned_manager.spawnedEnts = (EventSpawnedEntity *)gfc_allocate_array(sizeof(EventSpawnedEntity), numEnts);
	spawned_manager.max_spawns = numEnts;
	if (spawned_manager.spawnedEnts == NULL)
	{
		slog("Failed to allocate spawned ents list");
		spawned_manager_free();
	}
}

void spawned_manager_free()
{
	if (spawned_manager.spawnedEnts != NULL)
	{
		free(spawned_manager.spawnedEnts);
	}
	memset(&spawned_manager, 0, sizeof(EventSpawnedEntity));
}

void free_spawned_ent(char *tag)
{
	int i;

	for (i = 0; i < spawned_manager.max_spawns; ++i)
	{
		if (spawned_manager.spawnedEnts[i]._inuse == 0) continue;
		if (strcmp(spawned_manager.spawnedEnts[i].tag, tag) == 0)
		{
			entity_free(spawned_manager.spawnedEnts[i].ent);
			spawned_manager.spawnedEnts[i]._inuse = 0;
		}
	}
}

void store_spawned_ent(Entity *ent, const char *tag)
{
	int i;

	if (spawned_manager.spawnedEnts == NULL)
	{
		spawned_manager_init(10);
	}

	for (i = 0; i < spawned_manager.max_spawns; ++i)
	{
		if (spawned_manager.spawnedEnts[i]._inuse == 1) continue;
		memset(&spawned_manager.spawnedEnts[i], 0, sizeof(EventSpawnedEntity));
		spawned_manager.spawnedEnts[i]._inuse = 1;
		spawned_manager.spawnedEnts[i].ent = ent;
		spawned_manager.spawnedEnts[i].tag = (char *)malloc(strlen(tag) * sizeof(char));
		strcpy(spawned_manager.spawnedEnts[i].tag, tag);
		spawned_manager.spawnedEnts[i]. deathEventFile = NULL;
		return;
	}
	slog("No space for spawned ent");
	return;
}

void store_spawned_ent_with_death(Entity *ent, const char *tag, const char *deathEvent)
{
	int i;

	if (spawned_manager.spawnedEnts == NULL)
	{
		spawned_manager_init(10);
	}

	for (i = 0; i < spawned_manager.max_spawns; ++i)
	{
		if (spawned_manager.spawnedEnts[i]._inuse == 1) continue;
		memset(&spawned_manager.spawnedEnts[i], 0, sizeof(EventSpawnedEntity));
		spawned_manager.spawnedEnts[i]._inuse = 1;
		spawned_manager.spawnedEnts[i].ent = ent;
		spawned_manager.spawnedEnts[i].tag = (char *)malloc(strlen(tag) * sizeof(char));
		strcpy(spawned_manager.spawnedEnts[i].tag, tag);
		spawned_manager.spawnedEnts[i].deathEventFile = (char *)malloc(strlen(deathEvent) * sizeof(char));
		strcpy(spawned_manager.spawnedEnts[i].deathEventFile, deathEvent);
		return;
	}
	slog("No space for spawned ent");
	return;
}

void event_manager_update()
{
	int i;
	int clickX, clickY;
	Uint8 *state;

	if (event_manager.background_items == NULL)
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
	}


	if (event_manager.actors != NULL)
	{
		for (i = 0; i < event_manager.num_actors; ++i)
		{
			if (event_manager.actors[i]._inuse == 0) continue;
			cutscene_actor_update(&event_manager.actors[i]);
		}
	}


	//IF USER MOVE FORWARD, MOVE FORWARD

	if (event_manager.foreground_items == NULL || event_manager.background_items == NULL)
	{
		return;
	}

	state = SDL_GetKeyboardState(NULL);
	if (event_manager.action_wait > 0)
	{
		event_manager.action_wait--;
	}
	else if (state[SDL_SCANCODE_RETURN] || state[SDL_SCANCODE_KP_ENTER] || SDL_GetMouseState(&clickX, &clickY) & SDL_BUTTON(SDL_BUTTON_LEFT) || advance_scene == SDL_TRUE)
	{
		event_manager.action_wait = 30;
		event_manager.event_point++;
		next_event_point(event_manager.event_point);
	}

	if (event_manager.curr_action != NULL)
	{
		event_manager.curr_action();
	}
}

void event_manager_draw()
{
	int i;


	if (event_manager.actors != NULL)
	{
		for (i = 0; i < event_manager.num_actors; ++i)
		{
			if (event_manager.actors[i]._inuse == 0)
			{
				continue;
			}
			cutscene_actor_draw(&event_manager.actors[i]);
		}
	}

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


SDL_bool event_manager_active()
{
	if (event_manager.background_items == NULL)
	{
		return SDL_FALSE;
	}
	else
	{
		return SDL_TRUE;
	}
}

void next_event_point(int next_point)
{
	SJson *event_point;
	char *action_type;

	advance_scene = SDL_FALSE;

	if (next_point < 0 || next_point >= event_manager.event_length)
	{
		event_manager_clear();
		event_manager_free();
		resume_game();
		return;
	}
	event_manager_clear_notActor();
	event_point = sj_array_get_nth(event_manager.event_actions, next_point);

	event_manager.curr_action = NULL;

	action_type = sj_get_string_value(sj_object_get_value(event_point, "action"));
	if (strcmp(action_type, "dialogue") == 0)
	{
		start_dialogue_event_point(event_point);
	}
	else if (strcmp(action_type, "move") == 0)
	{
		start_movement_event_point(event_point);
	}
	else if (strcmp(action_type, "delete_actor") == 0)
	{
		start_deleteActor_event_point(event_point);
	}
	else if (strcmp(action_type, "spawn") == 0)
	{
		start_spawn_event_point(event_point);
	}
	else if (strcmp(action_type, "delete_ent") == 0)
	{
		start_deleteEnt_event_point(event_point);
	}
	else if (strcmp(action_type, "stop_bgmusic") == 0)
	{
		start_stop_bgmusic_point();
	}
	else if (strcmp(action_type, "start_bgmusic") == 0)
	{
		start_start_bgmusic_point(event_point);
	}
	else if (strcmp(action_type, "nothing") == 0)
	{
		advance_scene = SDL_TRUE;
	}

	event_manager.current_event = event_point;
	
	sj_get_integer_value(sj_object_get_value(event_point, "wait"), &event_manager.action_wait);
}

void start_dialogue_event_point(SJson *event_point)
{
	SJson *event_format, *colorjs, *linesjs, *spritejs;
	int x, y, sprite_w, sprite_h, sprite_fpl, sprite_num, frame_count, sprite_scale;
	int textx, texty;
	int ptsize, color_r, color_g, color_b, color_a;
	int linespacing, linecount, i;
	Color text_color;
	float sprite_frameRate;
	char *sprite_file, *fontFile, *speaker, *line;

	event_format = sj_load("defs/events/event_formats/dialogue_scene.json");
	if (event_format == NULL)
	{
		slog("Dialogue format not loading");
		return;
	}


	spritejs = sj_object_get_value(event_point, "sprite");
	if (spritejs != NULL)
	{
		sj_get_integer_value(sj_object_get_value(event_format, "speaker_spritex"), &x);
		sj_get_integer_value(sj_object_get_value(event_format, "speaker_spritey"), &y);
		sprite_file = sj_get_string_value(sj_object_get_value(spritejs, "file"));
		sj_get_integer_value(sj_object_get_value(spritejs, "sprite_w"), &sprite_w);
		sj_get_integer_value(sj_object_get_value(spritejs, "sprite_h"), &sprite_h);
		sj_get_integer_value(sj_object_get_value(spritejs, "sprite_fpl"), &sprite_fpl);
		sj_get_integer_value(sj_object_get_value(spritejs, "sprite_num"), &sprite_num);
		sj_get_float_value(sj_object_get_value(spritejs, "sprite_frameRate"), &sprite_frameRate);
		sj_get_integer_value(sj_object_get_value(spritejs, "frame_count"), &frame_count);
		sj_get_integer_value(sj_object_get_value(spritejs, "sprite_scale"), &sprite_scale);
		cutscene_item_spawn_sprite(sprite_file, sprite_w, sprite_h, sprite_fpl, sprite_num, sprite_frameRate, frame_count, sprite_scale, x, y, SDL_TRUE);
	}


	sprite_file = sj_get_string_value(sj_object_get_value(event_format, "backing"));
	sj_get_integer_value(sj_object_get_value(event_format, "x"), &x);
	sj_get_integer_value(sj_object_get_value(event_format, "y"), &y);
	sj_get_integer_value(sj_object_get_value(event_format, "sprite_w"), &sprite_w);
	sj_get_integer_value(sj_object_get_value(event_format, "sprite_h"), &sprite_h);
	sj_get_integer_value(sj_object_get_value(event_format, "sprite_fpl"), &sprite_fpl);
	sj_get_integer_value(sj_object_get_value(event_format, "sprite_num"), &sprite_num);
	sj_get_float_value(sj_object_get_value(event_format, "sprite_frameRate"), &sprite_frameRate);
	sj_get_integer_value(sj_object_get_value(event_format, "frame_count"), &frame_count);
	sj_get_integer_value(sj_object_get_value(event_format, "sprite_scale"), &sprite_scale);
	cutscene_item_spawn_sprite(sprite_file, sprite_w, sprite_h, sprite_fpl, sprite_num, sprite_frameRate, frame_count, sprite_scale, x, y, SDL_TRUE);


	fontFile = sj_get_string_value(sj_object_get_value(event_format, "namefont"));
	sj_get_integer_value(sj_object_get_value(event_format, "nameptsize"), &ptsize);

	colorjs = sj_object_get_value(event_format, "namecolor");
	if (colorjs != NULL)
	{
		sj_get_integer_value(sj_object_get_value(colorjs, "r"), &color_r);
		sj_get_integer_value(sj_object_get_value(colorjs, "g"), &color_g);
		sj_get_integer_value(sj_object_get_value(colorjs, "b"), &color_b);
		sj_get_integer_value(sj_object_get_value(colorjs, "a"), &color_a);
	}
	else
	{
		color_r = 0;
		color_g = 0;
		color_b = 0;
		color_a = 255;
	}
	sj_get_integer_value(sj_object_get_value(event_format, "namex"), &textx);
	sj_get_integer_value(sj_object_get_value(event_format, "namey"), &texty);

	text_color = gfc_color8(color_r, color_g, color_b, color_a);

	speaker = sj_get_string_value(sj_object_get_value(event_point, "speaker"));

	cutscene_item_spawn_text(speaker, fontFile, ptsize, text_color, x + textx, y + texty, SDL_FALSE);



	fontFile = sj_get_string_value(sj_object_get_value(event_format, "linefont"));
	sj_get_integer_value(sj_object_get_value(event_format, "lineptsize"), &ptsize);

	colorjs = sj_object_get_value(event_format, "linecolor");
	if (colorjs != NULL)
	{
		sj_get_integer_value(sj_object_get_value(colorjs, "r"), &color_r);
		sj_get_integer_value(sj_object_get_value(colorjs, "g"), &color_g);
		sj_get_integer_value(sj_object_get_value(colorjs, "b"), &color_b);
		sj_get_integer_value(sj_object_get_value(colorjs, "a"), &color_a);
	}
	else
	{
		color_r = 0;
		color_g = 0;
		color_b = 0;
		color_a = 255;
	}
	sj_get_integer_value(sj_object_get_value(event_format, "linesx"), &textx);
	sj_get_integer_value(sj_object_get_value(event_format, "linesy"), &texty);
	sj_get_integer_value(sj_object_get_value(event_format, "linespacing"), &linespacing);

	text_color = gfc_color8(color_r, color_g, color_b, color_a);

	linesjs = sj_object_get_value(event_point, "lines");
	if (linesjs != NULL)
	{
		linecount = sj_array_get_count(linesjs);
		for (i = 0; i < linecount; i++)
		{
			line = sj_get_string_value(sj_array_get_nth(linesjs, i));
			if (line == NULL)
			{
				slog("LINE Not found");
				continue;
			}
			cutscene_item_spawn_text(line, fontFile, ptsize, text_color, x + textx, y + texty + (linespacing * i), SDL_FALSE);
		}
	}
}

void start_movement_event_point(SJson *event_point)
{
	float speed;
	char *tag, *dir;
	int i;

	advance_scene = SDL_TRUE;
	sj_get_float_value(sj_object_get_value(event_point, "speed"), &speed);
	event_manager.action_speed = speed;
	
	tag = sj_get_string_value(sj_object_get_value(event_point, "actor"));
	for (i = 0; i < event_manager.num_actors; ++i)
	{
		if (event_manager.actors[i]._inuse == 0) continue;
		else if (strcmp(event_manager.actors[i].tag, tag) == 0)
		{
			event_manager.cur_actor = i;
			break;
		}
	}

	dir = sj_get_string_value(sj_object_get_value(event_point, "direction"));
	if (strcmp(dir, "north") == 0)
	{
		event_manager.curr_action = move_actor_north;
	}
	else if (strcmp(dir, "south") == 0)
	{
		event_manager.curr_action = move_actor_south;
	}
	else if (strcmp(dir, "east") == 0)
	{
		event_manager.curr_action = move_actor_east;
	}
	else if (strcmp(dir, "west") == 0)
	{
		event_manager.curr_action = move_actor_west;
	}
}

void start_deleteActor_event_point(SJson *event_point)
{
	char *tag;
	int i;

	advance_scene = SDL_TRUE;

	tag = sj_get_string_value(sj_object_get_value(event_point, "actor"));
	for (i = 0; i < event_manager.num_actors; ++i)
	{
		if (event_manager.actors[i]._inuse == 0) continue;
		else if (strcmp(event_manager.actors[i].tag, tag) == 0)
		{
			cutscene_actor_free(&event_manager.actors[i]);
			continue;
		}
	}
}


void start_spawn_event_point(SJson *event_point)
{
	Entity *ent;
	char *type, *tag, *deathEvent;
	int x, y;

	type = sj_get_string_value(sj_object_get_value(event_point, "type"));
	tag = sj_get_string_value(sj_object_get_value(event_point, "tag"));
	sj_get_integer_value(sj_object_get_value(event_point, "x"), &x);
	sj_get_integer_value(sj_object_get_value(event_point, "y"), &y);

	if (strcmp(type, "wall") == 0)
	{
		ent = wall_spawn(vector2d(x * 16 * 2, y * 16 * 2), "images/level_1_tiles.png", 1, 16, 16, 30, 2);
		store_spawned_ent(ent, tag);
	}
	else if (strcmp(type, "boss_one") == 0)
	{
		deathEvent = sj_get_string_value(sj_object_get_value(event_point, "deathEvent"));
		ent = boss1_spawn(vector2d(x * 16 * 2, y * 16 * 2));
		store_spawned_ent_with_death(ent, tag, deathEvent);
	}

	advance_scene = SDL_TRUE;
}

void start_deleteEnt_event_point(SJson *event_point)
{
	char *tag;

	tag = sj_get_string_value(sj_object_get_value(event_point, "tag"));
	free_spawned_ent(tag);

	advance_scene = SDL_TRUE;
}


void start_stop_bgmusic_point()
{
	gfc_stop_background_music();
	advance_scene = SDL_TRUE;
}

void start_start_bgmusic_point(SJson *event_point)
{
	char *string;
	Sound *music;

	advance_scene = SDL_TRUE;

	string = sj_get_string_value(sj_object_get_value(event_point, "music_file"));
	if (string)
	{
		music = gfc_sound_load(string, 1, -1);
		if (music == NULL)
		{
			slog("Music file could not be loaded");
			return;
		}
		else
		{
			gfc_set_background_music(music);
		}
	}
}

void event_manager_clear()
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
		cutscene_item_free(&event_manager.background_items[i]);
	}

	if (event_manager.foreground_items == NULL)
	{
		slog("Event manager not initialized");
		return;
	}
	for (i = 0; i < 5; ++i)
	{
		if (event_manager.foreground_items[i]._inuse == 0) continue;
		cutscene_item_free(&event_manager.foreground_items[i]);
	}

	if (event_manager.actors != NULL)
	{
		for (i = 0; i < event_manager.num_actors; ++i)
		{
			if (event_manager.actors[i]._inuse == 0) continue;
			cutscene_actor_free(&event_manager.actors[i]);
		}
	}
}


void event_manager_clear_notActor()
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
		cutscene_item_free(&event_manager.background_items[i]);
	}

	if (event_manager.foreground_items == NULL)
	{
		slog("Event manager not initialized");
		return;
	}
	for (i = 0; i < 5; ++i)
	{
		if (event_manager.foreground_items[i]._inuse == 0) continue;
		cutscene_item_free(&event_manager.foreground_items[i]);
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
		memset(&event_manager.background_items[i], 0, sizeof(CutsceneItem));
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
		memset(&event_manager.foreground_items[i], 0, sizeof(CutsceneItem));
		event_manager.foreground_items[i]._inuse = 1;
		event_manager.foreground_items[i].text = NULL;
		event_manager.foreground_items[i].font = NULL;
		event_manager.foreground_items[i].sprite = NULL;
		return &event_manager.foreground_items[i];
	}
	slog("No free entities available");
	return NULL;
}

CutsceneActor *cutscene_new_actor()
{
	int i;
	if (event_manager.actors == NULL)
	{
		slog("Event system does not exist");
		return NULL;
	}
	for (i = 0; i < event_manager.num_actors; ++i)
	{
		if (event_manager.actors[i]._inuse) continue; //Entity space is in use
		memset(&event_manager.actors[i], 0, sizeof(CutsceneActor));
		event_manager.actors[i]._inuse = 1;
		event_manager.actors[i].sprite = NULL;
		event_manager.actors[i].tag = NULL;
		return &event_manager.actors[i];
	}
	slog("No free entities available");
	return NULL;
}

CutsceneActor *cutscene_actor_spawn(const char *spriteFile, int sprite_w, int sprite_h, int sprite_fpl, int sprite_num, float sprite_frameRate, int frame_count, int sprite_scale, int x, int y, char *tag)
{
	CutsceneActor *actor;

	actor = cutscene_new_actor();
	if (actor == NULL)
	{
		slog("Cannot get actor");
		return NULL;
	}

	actor->position = vector2d(x * sprite_w * sprite_scale, y * sprite_h * sprite_scale);

	actor->sprite = gf2d_sprite_load_all(spriteFile, sprite_w, sprite_h, sprite_fpl);
	if (actor->sprite == NULL)
	{
		slog("Event sprite couldn't load");
		cutscene_actor_free(actor);
		return NULL;
	}
	actor->sprite_frame = sprite_num;
	actor->baseFrame = sprite_num;
	actor->frameCount = frame_count;
	actor->frameRate = sprite_frameRate;
	actor->scale = vector2d(sprite_scale, sprite_scale);

	actor->tag = (char *)malloc((strlen(tag)+1) * sizeof(char));
	strcpy(actor->tag, tag);

	return actor;
}

void cutscene_actor_free(CutsceneActor *self)
{
	if (self == NULL)
	{
		slog("Cannot free a NULL component");
		self->_inuse = 0;
		return;
	}
	if (self->sprite != NULL)
	{
		gf2d_sprite_free(self->sprite);
		self->sprite = NULL;
	}
	if (self->tag != NULL)
	{
		free(self->tag);
		self->sprite = NULL;
	}
	self->_inuse = 0;
}

void cutscene_actor_draw(CutsceneActor *self)
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
				(Uint32)self->sprite_frame);
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
				(Uint32)self->sprite_frame);
		}
	}
}

void cutscene_actor_update(CutsceneActor *self)
{
	if (!self) return;
	//Generic updates
	if (self->sprite != NULL)
	{
		self->sprite_frame += self->frameRate;
		if (self->sprite_frame >= self->frameCount + self->baseFrame) self->sprite_frame = self->baseFrame;
	}
}

void move_actor_south()
{
	if (event_manager.actors[event_manager.cur_actor]._inuse == 0) return;

	event_manager.actors[event_manager.cur_actor].position.y += event_manager.action_speed;
}

void move_actor_north()
{
	if (event_manager.actors[event_manager.cur_actor]._inuse == 0) return;

	event_manager.actors[event_manager.cur_actor].position.y -= event_manager.action_speed;
}

void move_actor_east()
{
	if (event_manager.actors[event_manager.cur_actor]._inuse == 0) return;

	event_manager.actors[event_manager.cur_actor].position.x += event_manager.action_speed;
}

void move_actor_west()
{
	if (event_manager.actors[event_manager.cur_actor]._inuse == 0) return;

	event_manager.actors[event_manager.cur_actor].position.x -= event_manager.action_speed;
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

CutsceneItem *cutscene_item_spawn_sprite(const char *spriteFile, int sprite_w, int sprite_h, int sprite_fpl, int sprite_num, float sprite_frameRate, int frame_count, int sprite_scale, int x, int y, SDL_bool background)
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
	item->baseFrame = sprite_num;
	item->frameCount = frame_count;
	item->frameRate = sprite_frameRate;
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
				(Uint32)self->sprite_frame);
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
				(Uint32)self->sprite_frame);
		}
	}
	if (self->text != NULL && self->font != NULL)
	{
		font_render(self->font, self->text, self->text_color, self->position);
	}


}

void cutscene_item_update(CutsceneItem *self)
{
	if (!self) return;
	//Generic updates
	if (self->sprite != NULL)
	{
		self->sprite_frame += self->frameRate;
		if (self->sprite_frame  >= self->frameCount + self->baseFrame) self->sprite_frame = self->baseFrame;
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
	evt->eventFile = (char *)malloc((strlen(eventFile) + 1) * sizeof(char));
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