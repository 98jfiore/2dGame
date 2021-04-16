#include <stdlib.h>
#include <SDL_keyboard.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "menu.h"

typedef struct
{
	MenuComponent *component_list; 
	Uint32 max_comp_across;
	Uint32 max_comp_down;
	Uint32 max_total_comp;
	Uint32 at_x;
	Uint32 at_y;
	Uint32 actionWait;
}MenuManager;

static MenuManager menu_manager = { 0 };

SDL_bool menu_manager_init(Uint32 max_comp_across, Uint32 max_comp_down)
{
	if (max_comp_across == 0 || max_comp_down == 0)
	{
		slog("Cannot allocate 0 components");
		return SDL_FALSE;
	}
	if (menu_manager.component_list != NULL)
	{
		menu_manager_free();
	}

	menu_manager.component_list = (MenuComponent *)gfc_allocate_array(sizeof(MenuComponent), max_comp_across * max_comp_down);
	if (menu_manager.component_list == NULL)
	{
		slog("Failed to allocate component list");
		return SDL_FALSE;
	}
	menu_manager.max_comp_across = max_comp_across;
	menu_manager.max_comp_down = max_comp_down;
	menu_manager.max_total_comp = max_comp_across * max_comp_down;
	menu_manager.actionWait = 40;
	menu_manager.at_x = 0;
	menu_manager.at_y = 0;
	atexit(menu_manager_free);
	slog("Menu initialized");
	return SDL_TRUE;
}

void menu_manager_set_pos(Uint32 x, Uint32 y)
{
	if (menu_manager.component_list == NULL)
	{
		slog("Cannot set position of null manager");
		return;
	}
	menu_manager.at_x = x;
	menu_manager.at_y = y;
}

Vector2D menu_manager_get_pos()
{
	return vector2d(menu_manager.at_x, menu_manager.at_y);
}

void menu_manager_free()
{
	if (menu_manager.component_list == NULL)
	{
		return;
	}
	free(menu_manager.component_list);
	memset(&menu_manager, 0, sizeof(MenuManager));
}

void menu_manager_update()
{
	int i;
	Uint8 *state;
	MenuComponent selected;

	if (menu_manager.component_list == NULL)
	{
		//slog("UI manager not initialized");
		return;
	}
	for (i = 0; i < menu_manager.max_total_comp; ++i)
	{
		if (menu_manager.component_list[i]._inuse == 0) continue;
		menu_component_update(&menu_manager.component_list[i]);
	}

	//Allow menu navigation
	state = SDL_GetKeyboardState(NULL);
	if (menu_manager.actionWait > 0)
	{
		menu_manager.actionWait--;
	}
	else if (state[SDL_SCANCODE_A])
	{
		if (menu_manager.at_x != 0)
		{
			menu_manager.component_list[menu_manager.at_y * menu_manager.max_comp_across + menu_manager.at_x].flags = 0;
			menu_manager.at_x--;
			menu_manager.component_list[menu_manager.at_y * menu_manager.max_comp_across + menu_manager.at_x].flags = MENU_SELECTED;
			menu_manager.actionWait = 30;
		}
	}
	else if (state[SDL_SCANCODE_D])
	{
		menu_manager.component_list[menu_manager.at_y * menu_manager.max_comp_across + menu_manager.at_x].flags = 0;
		menu_manager.at_x++;
		if (menu_manager.at_x >= menu_manager.max_comp_across) menu_manager.at_x = 0;
		else if (menu_manager.component_list[menu_manager.at_y * menu_manager.max_comp_across + menu_manager.at_x]._inuse == 0) menu_manager.at_x = 0;
		menu_manager.component_list[menu_manager.at_y * menu_manager.max_comp_across + menu_manager.at_x].flags = MENU_SELECTED;
		menu_manager.actionWait = 30;
	}
	else if (state[SDL_SCANCODE_W])
	{
		if (menu_manager.at_y != 0)
		{
			menu_manager.component_list[menu_manager.at_y * menu_manager.max_comp_across + menu_manager.at_x].flags = 0;
			menu_manager.at_y--;
			menu_manager.component_list[menu_manager.at_y * menu_manager.max_comp_across + menu_manager.at_x].flags = MENU_SELECTED;
			menu_manager.actionWait = 30;
		}
	}
	else if (state[SDL_SCANCODE_S])
	{
		menu_manager.component_list[menu_manager.at_y * menu_manager.max_comp_across + menu_manager.at_x].flags = 0;
		menu_manager.at_y++;
		if (menu_manager.at_y >= menu_manager.max_comp_down) menu_manager.at_y = 0;
		else if (menu_manager.component_list[menu_manager.at_y * menu_manager.max_comp_across + menu_manager.at_x]._inuse == 0) menu_manager.at_y = 0;
		menu_manager.component_list[menu_manager.at_y * menu_manager.max_comp_across + menu_manager.at_x].flags = MENU_SELECTED;
		menu_manager.actionWait = 30;
	}
	else if (state[SDL_SCANCODE_RETURN])
	{
		selected = menu_manager.component_list[menu_manager.at_y * menu_manager.max_comp_across + menu_manager.at_x];
		menu_manager.actionWait = 30;
		selected.action(&selected);
	}
}

void menu_manager_draw_components()
{
	int i;

	if (menu_manager.component_list == NULL)
	{
		//slog("UI manager not initialized");
		return;
	}
	for (i = 0; i < menu_manager.max_total_comp; ++i)
	{
		if (menu_manager.component_list[i]._inuse == 0) continue;
		menu_component_draw(&menu_manager.component_list[i]);
	}
}

MenuComponent *menu_component_new(Uint32 x, Uint32 y)
{
	if (menu_manager.component_list == NULL)
	{
		slog("UI system does not exist");
		return NULL;
	}
	if (x >= menu_manager.max_comp_across || y >= menu_manager.max_comp_down)
	{
		slog("New component out of bounds");
		return NULL;
	}
	if (menu_manager.component_list[y * menu_manager.max_comp_across + x]._inuse == 0)
	{
		memset(&menu_manager.component_list[y * menu_manager.max_comp_across + x], 0, sizeof(MenuComponent));
		menu_manager.component_list[y * menu_manager.max_comp_across + x]._inuse = 1;
		return &menu_manager.component_list[y * menu_manager.max_comp_across + x];
	}
	slog("Space not free");
	return NULL;
}

void menu_component_draw(MenuComponent *comp)
{
	Vector2D upperLeft;
	Uint32 frame;

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

		frame = (Uint32)comp->frame;
		if (comp->flags & MENU_SELECTED) frame++;

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
				frame);
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
				frame);
		}

		if (comp->text != NULL)
		{
			font_render(comp->text->font, comp->text->text, comp->text->text_color, comp->text->textPosition);
		}

		if (comp->second_sprite != NULL)
		{

			if (comp->second_sprite->second_scale.x != 0 || comp->second_sprite->second_scale.y != 0)
			{
				upperLeft = vector2d(0, 0);
				gf2d_sprite_draw(
					comp->second_sprite->second_sprite,
					comp->second_sprite->second_position,
					&comp->second_sprite->second_scale,
					&upperLeft,
					NULL,
					NULL,
					NULL,
					comp->second_sprite->second_frame);
			}
			else
			{
				gf2d_sprite_draw(
					comp->second_sprite->second_sprite,
					comp->second_sprite->second_position,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					comp->second_sprite->second_frame);
			}
		}
	}
}

void menu_component_update(MenuComponent *self)
{
	if (!self) return;
	//Generic updates
	self->frame += self->frameRate;
	if (self->frame >= self->frameCount + self->baseFrame) self->frame = self->baseFrame;
	//Specific updates
	if (self->update == NULL) return;
	self->update(self);
}

void menu_do_nothing(MenuComponent *self)
{
	printf("Nothing\n");
}

/*eol@eof*/