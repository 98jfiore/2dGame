#ifndef __UI_LIFE_H__
#define __UI_LIFE_H__

#include "gfc_vector.h"
#include "gf2d_sprite.h"

#include "entity.h"
#include "player.h"
#include "ui.h"

typedef struct
{
	Bool			_last;
	Player			*player;
	Sprite			*full_sprite;
	Sprite			*empty_sprite;
	int				healthRep;
}UILife;

typedef struct
{
	Player			*player;
	Vector4D		backdropColor;
	TextUIComponent	*text;
}UIGameOver;

/**
* @brief Frees the provided component
* @param ent The component to free
*/
void health_component_free(UIComponent *comp);

/**
* @brief Updates the provided component to the current state
* @param ent The component to update
*/
void health_component_update(UIComponent *self);

/**
* @brief Creates a health component from info given
* @param spriteFile The file the sprite can be found on
* @param sprite_w The width of sprites on the file
* @param sprite_h The height of sprites on the file
* @param sprite_fpl The number of sprites per like on the file
* @param sprite_count The number of frames the sprite is
* @param sprite_num The initial sprite this sprite can be found on
* @param spriteFile_empty The file the empty life sprite can be found on
* @param sprite_w_empty The width of sprites on the file
* @param sprite_h_empty The height of sprites on the file
* @param sprite_fpl_empty The number of sprites per like on the file
* @param sprite_count_empty The number of frames the sprite is
* @param sprite_num_empty  The initial sprite this sprite can be found on
* @param x The x position this component will be on
* @param y The y position this component will be on
* @param scale The scale this component is rendered at
* @param ent A pointer to the entity whose health you need to keep track of
* @return NULL on error or a pointer to a new UI Component
*/
UIComponent *health_component_create(const char *spriteFile, int sprite_w, int sprite_h, int sprite_fpl, int sprite_count, int sprite_num, const char *spriteFile_empty, int sprite_w_empty, int sprite_h_empty, int sprite_fpl_empty, int sprite_count_empty, int sprite_num_empty, int x, int y, int scale, Entity *ent);


/**
* @brief Frees the provided component
* @param ent The component to free
*/
void gameOver_component_free(UIComponent *comp);

/**
* @brief Updates the provided component to the current state
* @param ent The component to update
*/
void gameOver_component_update(UIComponent *self);

/**
* @brief Draws the provided component to the current state
* @param ent The component to update
*/
void gameOver_component_draw(UIComponent *self);

/**
* @brief Creates a game over component for the entity given
* @param ent A pointer to the entity whose health you need to keep track of
* @return NULL on error or a pointer to a new UI Component
*/
UIComponent *gameOver_component_create(Entity *ent);

#endif