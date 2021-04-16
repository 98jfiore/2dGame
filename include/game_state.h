#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include "SDL_stdinc.h"

#include "gfc_vector.h"
#include "gf2d_sprite.h"

enum gamestate_flags {
	GS_RESUME = 0,
	GS_PAUSED = 1,
	GS_QUIT = 2,
};


/**
* @brief Start or restart the game
*/
void start_game();

/**
* @brief Get the current state of the game
* @return What is the state of the game
*/
Uint8 get_gameState();

/**
* @brief Pause the game
*/
void pause_game();

/**
* @brief Pause the game
*/
void resume_game();

/**
* @brief Pause the game
*/
void quit_game();

#endif