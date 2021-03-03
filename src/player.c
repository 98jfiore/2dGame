#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "simple_logger.h"
#include "simple_json.h"

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
	player->maxhealth = 2;
	
	player->inventory = NULL;

	ent->data = player;

	ent->draw = player_draw;

	return ent;
}

void player_think(Entity *self)
{
	Uint8 *state;
	Vector2D new_vel;
	Player *player;
	int clickX, clickY;
	double clickAngle;

	//Move in a direction based on WASD
	new_vel = vector2d(0, 0);
	state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_A]) new_vel.x -= 5;
	if (state[SDL_SCANCODE_D]) new_vel.x += 5;
	if (state[SDL_SCANCODE_W]) new_vel.y -= 5;
	if (state[SDL_SCANCODE_S]) new_vel.y += 5;

	self->velocity = new_vel;

	player = (Player *)self->data;
	if (player == NULL) return;

	//Is left mouse button clicked
	if (SDL_GetMouseState(&clickX, &clickY) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		if ((~player->flags) & PLR_ATTACKING)
		{
			if (player->inventory != NULL && player->inventory->attack != NULL)
			{
				clickAngle = PointsAngle(self->position, vector2d(clickX, clickY));

				if (clickAngle >= 45 && clickAngle < 135)
				{
					player->attack = player->inventory->attack(self, MOV_NORTH);
				}
				else if (clickAngle >= 135 && clickAngle < 225)
				{
					player->attack = player->inventory->attack(self, MOV_WEST);
				}
				else if (clickAngle >= 225 && clickAngle < 315)
				{
					player->attack = player->inventory->attack(self, MOV_SOUTH);
				}
				else
				{
					player->attack = player->inventory->attack(self, MOV_EAST);
				}

				player->attackWait = 50;
				player->flags = player->flags | PLR_ATTACKING;
			}
		}
	}

	if (player->flags & PLR_ATTACKING && player->attack == NULL)
	{
		if (player->attackWait > 0)
		{
			player->attackWait--;
		}
		else
		{
			player->flags = player->flags & (~PLR_ATTACKING);
		}
	}
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
						save_playerUpgrade("saves/save.json", upgrade->tag);
						upgrade->action(collided, self);
						save_player(self, "saves/save.json");
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
		
		if (player->flags & PLR_ATTACKING && player->attack != NULL)
		{
			attack_update(player->attack);
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

	if (player->flags & PLR_ATTACKING && player->attack != NULL)
	{
		attack_draw(player->attack);
	}
}

void save_player(Entity *self, char *fileName)
{
	SJson *baseJson, *playerJson;
	Player *player;

	baseJson = sj_load(fileName);
	if (baseJson == NULL)
	{
		baseJson = sj_object_new();
	}

	//If player already saved in there, delete it
	sj_object_delete(baseJson, "player");

	playerJson = sj_object_new();
	if (playerJson == NULL)
	{
		slog("Could not create JSON object");
		return;
	}

	if (self == NULL)
	{
		slog("Cannot save a null player");
		return;
	}
	player = (Player *)self->data;
	if (player == NULL)
	{
		slog("Cannot save a null player");
		return;
	}

	sj_object_insert(playerJson, "health", sj_new_int(player->health));
	sj_object_insert(playerJson, "maxHealth", sj_new_int(player->maxhealth));

	sj_object_insert(baseJson, "player", playerJson);
	
	sj_save(baseJson, fileName);
}

void save_playerUpgrade(char *fileName, char *upgradeTag)
{
	SJson *baseJson;

	baseJson = sj_load(fileName);
	if (baseJson == NULL)
	{
		baseJson = sj_object_new();
	}

	if (upgradeTag == NULL)
	{
		slog("Cannot save NULL upgrade");
		return;
	}
	
	sj_object_delete(baseJson, upgradeTag);

	sj_object_insert(baseJson, upgradeTag, sj_new_int(1));

	sj_save(baseJson, fileName);
}

void load_player(Entity *ent, char *fileName)
{
	SJson *baseJson, *playerJson;
	int health, maxHealth;
	Player *player;

	if (ent == NULL)
	{
		slog("Cannot load player onto a NULL entity");
		return;
	}
	player = (Player *)ent->data;
	if (player == NULL)
	{
		slog("Cannot load player onto a non-player entity");
		return;
	}

	baseJson = sj_load(fileName);
	if (baseJson == NULL)
	{
		//slog("Could not load file");
		return;
	}

	playerJson = sj_object_get_value(baseJson, "player");
	if (playerJson == NULL)
	{
		slog("Could not load Player");
		return;
	}

	sj_get_integer_value(sj_object_get_value(playerJson, "health"), &health);
	sj_get_integer_value(sj_object_get_value(playerJson, "maxHealth"), &maxHealth);
	
	player->health = health;
	player->maxhealth = maxHealth;

	load_player_inventory(ent, baseJson);
}


void load_player_inventory(Entity *ent, SJson *save)
{
	SJson *obj;
	int objFound;
	Player *player;
	Inventory *inv;

	if ((~ent->flags) & ENT_PLAYER)
	{
		slog("Cannot load inventory of nonexistant player");
		return;
	}
	player = (Player *)ent->data;
	if (player == NULL)
	{
		slog("Cannot load inventory of nonexistant player");
		return;
	}

	//Does player have sword?
	obj = sj_object_get_value(save, "sword");
	if (obj != NULL)
	{
		sj_get_integer_value(obj, &objFound);
		if (objFound == 1)
		{
			inv = unlock_attack();
			inv->next = player->inventory;
			player->inventory = inv;
		}
	}
}

/*eol@eof*/
