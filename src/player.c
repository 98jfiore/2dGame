#include <SDL_keyboard.h>

#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_vector.h"

#include "player.h"
#include "entity.h"
#include "ent_item.h"
#include "ent_upgrade.h"

Entity *player_spawn(Vector2D position)
{
	Entity *ent;
	Rect *hitbox;
	Player *player;

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

	player = (Player *)malloc(sizeof(Player));
	player->flags = PLR_ALIVE;
	player->health = 2;
	player->maxhealth = 6;

	ent->data = player;

	ent->draw = player_draw;

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
	Player *player;
	Item *item;
	Upgrade *upgrade;
	int	damageTaken;

	player = (Player *)self->data;

	if (player->flags & PLR_ALIVE)
	{
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
					if ((~player->flags) & PLR_INVIN || collided->flags & ENT_NOINVIN)
					{
						damageTaken = collided->damage;
						if (damageTaken == -1)
						{
							player->health = 0;
							slog("Took all damage");
						}
						else
						{
							player->health -= damageTaken;
							slog("Took %i damage", damageTaken);
							player->flags = player->flags | PLR_INVIN;
							player->iframesRemaining = 80;
						}
					}
				}

				if (collided->flags & ENT_ITEM)
				{
					item = (Item *)collided->data;
					if (item->action != NULL)
					{
						item->action(collided, self);
					}
				}
				else if (collided->flags & ENT_UPGRADE)
				{
					upgrade = (Upgrade *)collided->data;
					if (upgrade->action != NULL)
					{
						upgrade->action(collided, self);
					}
				}
			}
		}

		if (player->flags & PLR_INVIN)
		{
			if (player->iframesRemaining > 0)
			{
				player->iframesRemaining--;
			}
			else
			{
				player->flags = PLR_ALIVE;
			}
		}
		
		if (player->health <= 0)
		{
			player->health = 0;
			player->flags = PLR_DEAD;
			return;
		}
		

		vector2d_add(self->position, self->position, self->velocity);
		player_think(self);
	}
	else if (player->flags & PLR_DEAD)
	{
		entity_free(self);
		slog("You DIED");
		return;
	}
}

void player_draw(Entity * self)
{
	Vector2D upperLeft;
	Player *player;

	player = (Player *)self->data;

	if (self->sprite == NULL || self->hitbox == NULL)
	{
		return; //Nothing to draw
	}

	if (player->flags & PLR_INVIN && player->iframesRemaining % 10 < 5)
	{
		//Flash when invincible.
		return;
	}

	if (self->scale.x != 0 || self->scale.y != 0)
	{
		upperLeft = vector2d(0, 0);
		gf2d_sprite_draw(
			self->sprite,
			self->position,
			&self->scale,
			&upperLeft,
			NULL,
			NULL,
			NULL,
			(Uint32)self->frame);
	}
	else
	{
		gf2d_sprite_draw(
			self->sprite,
			self->position,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			(Uint32)self->frame);
	}
}

/*eol@eof*/
