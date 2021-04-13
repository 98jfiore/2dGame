#ifndef __MENU_H__
#define __MENU_H__

#include "SDL_stdinc.h"

#include "gfc_vector.h"
#include "gf2d_sprite.h"

#include "entity.h"
#include "player.h"
#include "font.h"

enum menu_flags {
	MENU_SELECTED = 1,
};

typedef struct MenuComponent_s
{
	Bool			_inuse;
	Uint8			flags;
	Sprite			*sprite;
	Vector2D		position;
	Vector2D		textPosition;
	Vector2D		scale;
	Vector4D		*color;
	float			frame;
	float			frameRate;
	int				frameCount;
	int				baseFrame;
	char			*text;
	Font			*font;
	Color			text_color;
	void			(*update)(struct MenuComponent_s *self);
	void			(*draw)(struct MenuComponent_s *self);
	void			(*free)(struct MenuComponent_s *self);
	void			(*action)(char *arg);
	char			*action_specification;
	void			*data;
}MenuComponent;

/**
* @brief Initialize the ui manager
* @param max_comp_across How many components wide can the component be
* @param max_comp_across How many components down can the component be
*/
SDL_bool menu_manager_init(Uint32 max_comp_across, Uint32 max_comp_down);

/**
* @brief Free all components in the system and destroy the menu manager
*/
void menu_manager_free();

/**
* @brief Update all menu components in the system
*/
void menu_manager_update();

/**
* @brief Draw all menu components in the system
*/
void menu_manager_draw_components();

/**
* @brief Allocate a component in the component list and return a pointer to it.
* @param x The x position to allocate this component
* @param y The y position to allocate this component
* @return NULL on error or a pointer to an initialized component.
*/
MenuComponent *menu_component_new(Uint32 x, Uint32 y);

/**
* @brief Draws the provided menu component to the current render frame
* @param ent The component to draw
*/
void menu_component_draw(MenuComponent *comp);

/**
* @brief Updates the provided menu component to the current state
* @param ent The component to update
*/
void menu_component_update(MenuComponent *self);

/**
* @brief Have components without specific knowledge do something
* @param arg A string that does nothing
*/
void menu_do_nothing(char *arg);

#endif