#ifndef __UI_H__
#define __UI_H__

#include "gfc_vector.h"
#include "gf2d_sprite.h"

#include "entity.h"
#include "player.h"

enum ui_flags {
	UI_INVISIBLE = 1,
};

typedef struct UIComponent_s
{
	Bool			_inuse;
	Uint8			flags;
	Sprite			*sprite;
	Vector2D		position;
	Vector2D		scale;
	float			frame;
	float			frameRate;
	int				frameCount;
	int				baseFrame;
	void			(*update)(struct UIComponent_s *self);
	void			(*draw)(struct UIComponent_s *self);
	void			(*free)(struct UIComponent_s *self);
	void			*data;
}UIComponent;

/**
* @brief Initialize the ui manager
* @param max_comp How many concurrent ui components to support
*/
void ui_manager_init(Uint32 max_comp);

/**
* @brief Free all components in the system and destroy the ui manager
*/
void ui_manager_free();

/**
* @brief Update all ui components in the system
*/
void ui_manager_update_components();

/**
* @brief Draw all ui components in the system
*/
void ui_manager_draw_components();

/**
* @brief Allocate a component in the component list and return a pointer to it.
* @return NULL on error or a pointer to an initialized component.
*/
UIComponent *component_new();

/**
* @brief Load a ui format
* @param filename Name of file ui setup is saved on.
*/
void ui_format_load(const char *filename, Entity *player);

/**
* @brief Frees the provided component
* @param ent The component to free
*/
void component_free(UIComponent *comp);

/**
* @brief Draws the provided component to the current render frame
* @param ent The component to draw
*/
void component_draw(UIComponent *comp);

/**
* @brief Updates the provided component to the current state
* @param ent The component to update
*/
void component_update(UIComponent *self);

/**
* @brief Creates a component from info given
* @param spriteFile The file the sprite can be found on
* @param sprite_w The width of sprites on the file
* @param sprite_h The height of sprites on the file
* @param sprite_fpl The number of sprites per like on the file
* @param sprite_count The number of frames the sprite is
* @param sprite_num The initial sprite this sprite can be found on
* @param x The x position this component will be on
* @param y The y position this component will be on
* @param scale The scale this component is rendered at
* @return NULL on error or a pointer to a new UI Component
*/
UIComponent *component_create(const char *spriteFile, int sprite_w, int sprite_h, int sprite_fpl, int sprite_count, int sprite_num, int x, int y, int scale);

#endif