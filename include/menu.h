#ifndef __MENU_H__
#define __MENU_H__

#include "SDL_stdinc.h"

#include "gfc_vector.h"
#include "gf2d_sprite.h"

#include "entity.h"
#include "player.h"
#include "font.h"
#include "game_state.h"

enum menu_flags {
	MENU_SELECTED = 1,
};

typedef struct
{
	Sprite			*second_sprite;
	float			second_frame;
	float			second_frameRate;
	int				second_frameCount;
	int				second_baseFrame;
	Vector2D		second_scale;
	Vector2D		second_position;
}SecondSprite;

typedef struct
{
	Vector2D		textPosition;
	char			*text;
	Font			*font;
	Color			text_color;
}MenuText;

typedef struct MenuComponent_s
{
	Bool			_inuse;
	Uint8			flags;
	Sprite			*sprite;
	Vector2D		position;
	Vector2D		scale;
	Vector4D		*color;
	float			frame;
	float			frameRate;
	int				frameCount;
	int				baseFrame;
	SecondSprite	*second_sprite;
	MenuText		*text;
	void			(*update)(struct MenuComponent_s *self);
	void			(*draw)(struct MenuComponent_s *self);
	void			(*free)(struct MenuComponent_s *self);
	void			(*action)(struct MenuComponent_s *self);
	char			*action_specification;
	void			*data;
}MenuComponent;

/**
* @brief Initialize the ui manager
* @param max_comp_across How many components wide can the component be
* @param max_comp_across How many components down can the component be
* @return Did the menu manager initialize
*/
SDL_bool menu_manager_init(Uint32 max_comp_across, Uint32 max_comp_down);

/**
* @brief Is there a menu manager active
* @return Boolean referencing if there is a menu
*/
SDL_bool menu_manager_active();

/**
* @brief Is there a menu manager active
* @gsflags A pointer to the gamestate flags
*/
void menu_manager_setFlags(Uint8 *gsflags);

/**
* @brief Set the menu_manager's at value to something specific
* @param x The x value to set at_x to
* @param y The y value to set at_y to
*/
void menu_manager_set_pos(Uint32 x, Uint32 y);

/**
* @brief Get the position of the menu manager
* @return The current position of the menu_manager
*/
Vector2D menu_manager_get_pos();

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
* @brief Resume the game
* @param ent The component that called the function
*/
void menu_resume_game(MenuComponent *self);

/**
* @brief Updates the provided menu component to the current state
* @param ent The component that called the function
*/
void menu_quit_game(MenuComponent *self);

/**
* @brief Have components without specific knowledge do something
* @param self The component that does nothing
*/
void menu_do_nothing(MenuComponent *self);

#endif