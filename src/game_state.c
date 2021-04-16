#include <stdlib.h>
#include <SDL_keyboard.h>

#include "simple_logger.h"
#include "simple_json.h"

#include "game_state.h"

static Uint8 state = 0;

void start_game()
{
	state = 0;
}

Uint8 get_gameState()
{
	return state;
}

void pause_game()
{
	state = GS_PAUSED;
}

void resume_game()
{
	state = 0;
}

void quit_game()
{
	state = GS_QUIT;
}

/*eol@eof*/