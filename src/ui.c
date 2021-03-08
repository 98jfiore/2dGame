#include <stdlib.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "font.h"

#include "ui.h"
#include "ui_life.h"

typedef struct
{
	UIComponent *component_list;
	Uint32 max_components;
}UIManager;

static UIManager ui_manager = { 0 };

void ui_manager_init(Uint32 max_comp)
{
	if (max_comp == 0)
	{
		slog("Cannot allocate 0 components");
		return;
	}
	if (ui_manager.component_list != NULL)
	{
		ui_manager_free();
	}

	ui_manager.component_list = (UIComponent *)gfc_allocate_array(sizeof(UIComponent), max_comp);
	if (ui_manager.component_list == NULL)
	{
		slog("Failed to allocate component list");
		return;
	}
	ui_manager.max_components = max_comp;
	atexit(ui_manager_free);
	slog("UI system initialized");

	//gfc_text_init("fonts/colony_wars.ttf");
}

void ui_manager_free()
{
	if (ui_manager.component_list != NULL)
	{
		free(ui_manager.component_list);
	}
	memset(&ui_manager, 0, sizeof(UIManager));
}

void ui_manager_update_components()
{
	int i;
	if (ui_manager.component_list == NULL)
	{
		slog("Entity manager not initialized");
		return;
	}
	for (i = 0; i < ui_manager.max_components; ++i)
	{
		if (ui_manager.component_list[i]._inuse == 0) continue;
		component_update(&ui_manager.component_list[i]);
	}
}

void component_update(UIComponent *self)
{
	if (!self) return;
	//Generic updates
	self->frame += self->frameRate;
	if (self->frame >= self->frameCount + self->baseFrame) self->frame = self->baseFrame;
	//Specific updates
	if (self->update == NULL) return;
	self->update(self);
}

void ui_manager_draw_components()
{
	int i;
	Font *font;
	if (ui_manager.component_list == NULL)
	{
		slog("Entity manager not initialized");
		return;
	}
	for (i = 0; i < ui_manager.max_components; ++i)
	{
		if (ui_manager.component_list[i]._inuse == 0) continue;
		component_draw(&ui_manager.component_list[i]);
	}
	//gfc_text_draw_line("HEY", FT_H1, gfc_color(126, 104, 166, 1), vector2d(20,20));
	/*font = font_load("fonts/RETRO_SPACE.ttf", 70);
	font_render(font, "GAME OVER", gfc_color8(0, 0, 0, 255), vector2d(500, 300));
	font_render(font, "GAME OVER", gfc_color8(0, 0, 0, 150), vector2d(500, 200));*/
}

void component_draw(UIComponent *comp)
{
	Vector2D upperLeft;

	if (comp == NULL)
	{
		slog("Cannot draw a NULL component");
		return;
	}
	//If specialized draw, do it, otherwise do generic
	if (comp->draw)
	{
		comp->draw(comp);
	}
	else
	{
		if (comp->sprite == NULL)
		{
			return; //Nothing to draw
		}
		if (comp->scale.x != 0 || comp->scale.y != 0)
		{
			upperLeft = vector2d(0, 0);
			gf2d_sprite_draw(
				comp->sprite,
				comp->position,
				&comp->scale,
				&upperLeft,
				NULL,
				NULL,
				NULL,
				(Uint32)comp->frame);
		}
		else
		{
			gf2d_sprite_draw(
				comp->sprite,
				comp->position,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				(Uint32)comp->frame);
		}
	}
}

UIComponent *component_new()
{
	int i;
	if (ui_manager.component_list == NULL)
	{
		slog("UI system does not exist");
		return NULL;
	}
	for (i = 0; i < ui_manager.max_components; ++i)
	{
		if (ui_manager.component_list[i]._inuse) continue; //Entity space is in use
		memset(&ui_manager.component_list[i], 0, sizeof(UIComponent));
		ui_manager.component_list[i]._inuse = 1;
		return &ui_manager.component_list[i];
	}
	slog("No free components available");
	return NULL;
}

UIComponent *component_create(const char *spriteFile, int sprite_w, int sprite_h, int sprite_fpl, int sprite_count, int sprite_num, int x, int y, int scale)
{
	UIComponent *comp;

	comp = component_new();
	if (comp == NULL)
	{
		slog("Failed to create component");
		return NULL;
	}

	comp->position = vector2d(x, y);
	comp->sprite = gf2d_sprite_load_all(spriteFile, sprite_w, sprite_h, sprite_fpl);
	if (comp->sprite == NULL)
	{
		slog("UI sprite couldn't load");
		free(comp);
		return NULL;
	}
	comp->frameCount = sprite_count;
	comp->baseFrame = sprite_num;
	comp->scale = vector2d(scale, scale);
	comp->frameRate = 0;

	return comp;
}

void component_free(UIComponent *comp)
{
	if (comp == NULL)
	{
		slog("Cannot free a NULL component");
		return;
	}
	if (comp->free != NULL)
	{
		comp->free(comp);
	}
	gf2d_sprite_free(comp->sprite);
	comp->sprite = NULL;
	comp->_inuse = 0;
}

void ui_format_load(const char *filename, Entity *player)
{
	SJson *json, *uijs;
	SJson *compsjs, *compjs, *comp_infojs, *secondaryjs;
	const char *sprite_file;
	const char *sprite_file_sec;
	int sprite_w, sprite_h, sprite_fpl, sprite_fc, sprite_fn, sprite_scale;
	int sprite_w_sec, sprite_h_sec, sprite_fpl_sec, sprite_fc_sec, sprite_fn_sec;
	int compCount;
	int compx, compy;
	const char * compType;
	int i;

	if (filename == NULL)
	{
		slog("Need filename to load ui setup");
		return;
	}

	json = sj_load(filename);
	if (json == NULL)
	{
		slog(filename);
		slog("File could not be loaded");
		return;
	}

	ui_manager_init(20);
	if (ui_manager.component_list == NULL)
	{
		slog("UI system could not be created");
		sj_free(json);
		return;
	}

	uijs = sj_object_get_value(json, "ui");
	if (uijs == NULL)
	{
		slog("UI json missing ui object");
		ui_manager_free();
		sj_free(json);
		return;
	}

	compsjs = sj_object_get_value(uijs, "components");
	if (compsjs != NULL)
	{
		compCount = sj_array_get_count(compsjs);
		for (i = 0; i < compCount; ++i)
		{
			compjs = sj_array_get_nth(compsjs, i);
			if (compjs == NULL)
			{
				slog("Component not found");
				return;
			}

			compType = sj_get_string_value(sj_object_get_value(compjs, "type"));
			sj_get_integer_value(sj_object_get_value(compjs, "x"), &compx);
			sj_get_integer_value(sj_object_get_value(compjs, "y"), &compy);
			comp_infojs = sj_object_get_value(uijs, compType);
			if (comp_infojs == NULL)
			{
				slog("Component type not found");
				return;
			}

			sprite_file = sj_get_string_value(sj_object_get_value(comp_infojs, "filename"));
			sj_get_integer_value(sj_object_get_value(comp_infojs, "sprite_w"), &sprite_w);
			sj_get_integer_value(sj_object_get_value(comp_infojs, "sprite_h"), &sprite_h);
			sj_get_integer_value(sj_object_get_value(comp_infojs, "frames_per_line"), &sprite_fpl);
			sj_get_integer_value(sj_object_get_value(comp_infojs, "frame_count"), &sprite_fc);
			sj_get_integer_value(sj_object_get_value(comp_infojs, "frame_num"), &sprite_fn);
			sj_get_integer_value(sj_object_get_value(comp_infojs, "sprite_scale"), &sprite_scale);

			if (strcmp(compType, "life") == 0)
			{
				secondaryjs = sj_object_get_value(comp_infojs, "empty life");
				sprite_file_sec = sj_get_string_value(sj_object_get_value(secondaryjs, "filename"));
				sj_get_integer_value(sj_object_get_value(secondaryjs, "sprite_w"), &sprite_w_sec);
				sj_get_integer_value(sj_object_get_value(secondaryjs, "sprite_h"), &sprite_h_sec);
				sj_get_integer_value(sj_object_get_value(secondaryjs, "frames_per_line"), &sprite_fpl_sec);
				sj_get_integer_value(sj_object_get_value(secondaryjs, "frame_count"), &sprite_fc_sec);
				sj_get_integer_value(sj_object_get_value(secondaryjs, "frame_num"), &sprite_fn_sec);
				
				health_component_create(sprite_file, sprite_w, sprite_h, sprite_fpl, sprite_fc, sprite_fn, sprite_file_sec, sprite_w_sec, sprite_h_sec, sprite_fpl_sec, sprite_fc_sec, sprite_fn_sec, compx, compy, sprite_scale, player);
			}
			else
			{
				component_create(sprite_file, sprite_w, sprite_h, sprite_fpl, sprite_fc, sprite_fn, compx, compy, sprite_scale);
			}
		}
	}


	sj_free(json);
}


/*eol@eof*/