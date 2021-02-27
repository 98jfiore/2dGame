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

	ent->sprite = gf2d_sprite_load_all("images/player.png", 16, 16, 2);
	ent->frameCount = 2;
	ent->frameRate = 0.05;
	ent->baseFrame = 0;
	ent->scale = vector2d(2,2);

	ent->flags = ENT_PLAYER;

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x;
	hitbox->y = position.y;
	hitbox->width = ent->sprite->frame_w * 2;
	hitbox->height = ent->sprite->frame_h * 2;
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
	if (state[SDL_SCANCODE_A]) new_vel.x -= 5;
	if (state[SDL_SCANCODE_D]) new_vel.x += 5;
	if (state[SDL_SCANCODE_W]) new_vel.y -= 5;
	if (state[SDL_SCANCODE_S]) new_vel.y += 5;

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
			if (collided->flags & ENT_SOLID)
			{
				self->hitbox->x -= self->velocity.x;
				self->hitbox->y -= self->velocity.y;
				self->velocity = vector2d(0, 0);
				//printf("%.6f, %.6f vs %.6f, %.6f\n", self->hitbox->x, self->hitbox->y, self->position.x, self->position.y);
				//printf("%.6f, %.6f vs %.6f, %.6f\n", collided->hitbox->x, collided->hitbox->y, collided->position.x, collided->position.y);
			}
			else if (collided->flags & ENT_DEADLY)
			{
				entity_free(self);
				slog("You DIED");
				return;
				//printf("%.6f, %.6f vs %.6f, %.6f\n", self->hitbox->x, self->hitbox->y, self->position.x, self->position.y);
				//printf("%.6f, %.6f vs %.6f, %.6f\n", collided->hitbox->x, collided->hitbox->y, collided->position.x, collided->position.y);
			}
		}
	}


	vector2d_add(self->position, self->position, self->velocity);
	player_think(self);
}