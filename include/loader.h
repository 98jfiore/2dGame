#ifndef __LOADER_H__
#define __LOADER_H__

#include "SDL_stdinc.h"

#include "gfc_types.h"
#include "gfc_text.h"

#include "gf2d_sprite.h"

#include "entity.h"
#include "level.h"
#include "menu.h"


typedef struct
{
	char *nextLevel;
	char *nextPos;
	Level *currentLevel;
}Transition;

/**
* @brief Load a level
* @param filename Name of file level is saved on.
* @return NULL on error, or a pointer to the loaded level.
*/
Level *level_load(const char *filename);

/**
* @brief Load a level
* @param filename Name of file level is saved on.
* @param player An entity pointing to an already loaded player
* @param nextPlayerPos A string containing the title of the next next player position
* @return NULL on error, or a pointer to the loaded level.
*/
Level *level_loadWithPlayer(const char *filename, Entity *player, char *nextPlayerPos);

/**
* @brief Load the basic information from a level json without any player loading
* @param filename Name of file level is saved on.
* @return NULL on error, or a pointer to the loaded level.
*/
Level *level_jsonload(const char *filename);

/**
* @brief Unload a level=
* @param level A pointer to the current level.
* @return NULL on error or no player, else return a pointer to the player entity.
*/
Entity *level_unload(Level *level);

/**
* @brief Draw a level
*/
void level_draw();

/**
* @brief Unload a level and load the next
* @param currentLevel A pointer to the current level.
*/
void level_transition(const char *nextLevel);

/**
* @brief Unload a level and load the next level without a save
* @param currentLevel A pointer to the current level.
*/
void level_transitionNewGame(const char *nextLevel);

/**
* @brief Unload a level and load the next
* @param nextLevel A string containing the title of the next level
* @param nextPos A string containing the title of the next next player position
* @param currentLevel A pointer to the current level.
*/
void level_transitionWithPlayer(const char *nextLevel, char *nextPos, Level *currentLevel);

/**
* @brief Spawn a transition entity
* @param position The screen position to spawn the transitoin object at.
* @param spriteSheet The name of the file the sprite we are looking for can be found at.
* @param frameNum The frame of the sprite we are looking for.
* @param spriteWidth The width of sprites in the sprite sheet.
* @param spriteHeight The height of sprites in the sprite sheet.
* @param spriteHeight Frames per line in the sprite sheet.
* @param scale How much to scale the sprite by.
* @param nextLevel The name of the next level you want to transition to.
* @param nextPos The title of the position to transition to.
* @param currentLevel A pointer to the current level.
* @return NULL on error, or a pointer to a new transition entity.
*/
Entity *transition_spawn(Vector2D position, char *spriteSheet, int frameNum, int spriteWidth, int spriteHeight, int fpl, int scale, char *nextLevel, char *nextPos, Level *currentLevel);

/**
* @brief Update a moving wall entity
* @param self The wall that is to be updated.
*/
void transition_update(Entity *self);

/**
* @brief Free a transition entity
* @param self The door that is to be updated.
*/
void transition_free(Entity *self);




/**
* @brief Load a menu format
* @param filename Name of file ui setup is saved on.
*/
void menu_format_load(const char *filename);

/**
* @brief Creates a component containing text from info given
* @param text The text that will show up in the componenet
* @param fontFile The file that the font can be found on
* @param text_ptsize The pointsize of the text
* @param text_color The color of the text
* @param spriteFile The file the sprite can be found on
* @param sprite_w The width of sprites on the file
* @param sprite_h The height of sprites on the file
* @param sprite_fpl The number of sprites per like on the file
* @param sprite_count The number of frames the sprite is
* @param sprite_num The initial sprite this sprite can be found on
* @param sprite_scale The scale this component is rendered at
* @param menu_x The x position in the menu this component will be on
* @param menu_y The y position in the menu this component will be on
* @param offset_x The x offset of the text within the comp
* @param offset_y The y offset of the text within the comp
* @param x The x position this component will be on
* @param y The y position this component will be on
* @param action The name of the action the component will do when selected
* @param scale The scale this component is rendered at
* @param specification A specification for the action performed
* @return NULL on error or a pointer to a new UI Component
*/
MenuComponent *menu_component_create(char *text, char *fontFile, Uint32 text_ptsize, Color text_color, const char *spriteFile, int sprite_w, int sprite_h, int sprite_fpl, int sprite_count, int sprite_num, int sprite_scale, int menu_x, int menu_y, int offset_x, int offset_y, int x, int y, char *action, char *specification);



#endif