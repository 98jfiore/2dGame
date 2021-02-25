#include <SDL_keyboard.h>

#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_vector.h"

#include "player.h"
#include "entity.h"

Entity *player_spawn(Vector2D position)
{
	Entity *ent;
	Rect *hitbox;

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

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x;
	hitbox->y = position.y;
	hitbox->width = ent->sprite->frame_w;
	hitbox->height = ent->sprite->frame_h;
	ent->hitbox = hitbox;

	ent->update = player_update;

	return ent;
}

void player_think(Entity *self)
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

void player_update(Entity *self)
{
	Entity *collided;

	if (self->hitbox != NULL)
	{
		self->hitbox->x += self->velocity.x;
		self->hitbox->y += self->velocity.y;
		//Check if player is coliding with something, find out what it is, and do something about it.
		collided = check_collision(self);
		if (collided)
		{
			printf("COLLIDED");
		}
	}


	vector2d_add(self->position, self->position, self->velocity);
	player_think(self);
}