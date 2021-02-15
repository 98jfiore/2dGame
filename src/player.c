#include <SDL_keyboard.h>

#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_vector.h"

#include "player.h"

Entity *player_spawn(Vector2D position)
{
	Entity *ent;

	ent = entity_new();
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->sprite = gf2d_sprite_load_all("images/ed210_top.png", 128, 128, 16);
	ent->frameCount = 16;
	ent->frameRate = 0.1;

	ent->update = player_update;

	return ent;
}

void player_update(Entity *self)
{
	Uint8 *state;
	Vector2D new_vel;

	//Move in a direction based on WASD
	new_vel = vector2d(0, 0);
	state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_A]) new_vel.x -= 10;
	if (state[SDL_SCANCODE_D]) new_vel.x += 10;
	if (state[SDL_SCANCODE_W]) new_vel.y -= 10;
	if (state[SDL_SCANCODE_S]) new_vel.y += 10;

	self->velocity = new_vel;
}