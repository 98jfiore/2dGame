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
#include "ent_environ.h"

static char *playerFile = "saves/save.json";
static char *saveFile = "";

Entity *player_spawn(Vector2D position)
{
	Entity *ent;
	Rect *hitbox;
	Player *player;

	ent = entity_new_back();
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
	player->attack = NULL;

	player->attackWait = 20;
	player->specialWait = 0;

	player->attack_sound = gfc_sound_load("music/swish_4.mp3", 0.75, 2);

	ent->data = player;

	ent->draw = player_draw;

	return ent;
}

void player_think(Entity *self)
{
	Uint8 *state;
	Vector2D new_vel;
	Player *player;
	Inventory *inv;
	int clickX, clickY;
	double clickAngle;

	//Move in a direction based on WASD
	//If player is not in knockback
	if (self->velocity.x <= 5 && self->velocity.y <= 5 && self->velocity.x >= -5 && self->velocity.y >= -5)
	{
		new_vel = vector2d(0, 0);
		state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_A]) new_vel.x -= 5;
		else if (state[SDL_SCANCODE_D]) new_vel.x += 5;
		else if (state[SDL_SCANCODE_W]) new_vel.y -= 5;
		else if (state[SDL_SCANCODE_S]) new_vel.y += 5;
	}
	else
	{
		new_vel = self->velocity;
		new_vel.x = new_vel.x * 2 / 3;
		new_vel.y = new_vel.y * 2 / 3;
	}

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

				if (player->attack_sound != NULL) gfc_sound_play(player->attack_sound, 0, 0.35, 2, -1);

				player->flags = player->flags | PLR_ATTACKING;
			}
		}
	}

	//Is right mouse button clicked
	if (SDL_GetMouseState(&clickX, &clickY) & SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		//Does the player even have bombs?
		if ((~player->flags) & PLR_ATTACKING && player->specialWait <= 0)
		{
			inv = player->inventory;
			while (inv != NULL)
			{
				if (strcmp(inv->name, "bombs") == 0)
				{
					drop_bomb(self);
					player->attackWait = 50;
					player->specialWait = 500;
					player->flags = player->flags | PLR_ATTACKING;
					player->attack = NULL;
					break;
				}

				inv = inv->next;
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

	if (player->specialWait > 0)
	{
		player->specialWait--;
	}
}

void player_update(Entity *self)
{
	Entity *collided;
	Player *player;
	Item *item;
	Upgrade *upgrade;
	Door *door;
	Inventory *inv;
	SDL_bool happened;
	char *grabbedTag;
	int	damageTaken;

	player = (Player *)self->data;
	if (player == NULL)
	{
		slog("Player is NULL");
		return;
	}


	if (player->flags & PLR_ATTACKING && player->attack != NULL)
	{
		attack_update(player->attack);
	}

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
				//If you collided with a door, see if you can open it
				if (collided->flags & ENT_LOCKED)
				{
					door = (Door *)collided->data;
					if (door == NULL)
					{
						slog("Locked object does not have information on that");
						return;
					}

					//Go through inventory
					inv = player->inventory;
					while (inv != NULL)
					{
						if (strcmp(inv->name, door->keyType) == 0)
						{
							entity_free(collided);
							break;
						}

						inv = inv->next;
					}
				}
				
				if (collided != NULL)
				{
					if (collided->flags & ENT_SOLID)
					{
						self->hitbox->x -= self->velocity.x;
						self->hitbox->y -= self->velocity.y;
						self->velocity = vector2d(0, 0);
					}
					else if (collided->flags & ENT_DEADLY)
					{
						if ((self->velocity.x > 5 || self->velocity.x < -5 || self->velocity.y > 5 || self->velocity.y < -5) && collided->flags & ENT_NOINVIN)
						{

						}
						else if ((~player->flags) & PLR_INVIN || collided->flags & ENT_NOINVIN)
						{
							damageTaken = collided->damage;
							if (damageTaken == -1)
							{
								player->health = 0;
								slog("Took all damage");
							}
							else
							{
								player_doDamage(self, damageTaken);
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
							grabbedTag = upgrade->tag;
							happened = upgrade->action(collided, self);
							if (happened == SDL_TRUE)
							{
								save_playerUpgrade(playerFile, grabbedTag);
							}
							save_player(self, playerFile);
						}
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
				player->flags = player->flags ^ PLR_INVIN;
			}
		}
		
		if (player->health <= 0)
		{
			player->health = 0;
			player->flags = PLR_DEAD;
			return;
		}
		vector2d_add(self->position, self->position, self->velocity);

		//printf("%.6f, %.6f   %.6f, %.6f\n", self->hitbox->x, self->hitbox->y, self->position.x, self->position.y);

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

void player_free(Entity *self)
{
	Player *player;

	if (self == NULL)
	{
		slog("Cannot free a null ent");
		return;
	}
	player = (Player *)self->data;
	if (player == NULL)
	{
		slog("Entity is not a player");
		return;
	}
	attack_free(player->attack);
	free_inventory(player->inventory);
	free(player);
}


void player_doDamage(Entity *player, int damage)
{
	Player *plr;

	if (player == NULL)
	{
		slog("Cannot damage a null ent");
		return;
	}
	plr = (Player *)player->data;
	if ((~player->flags) & ENT_PLAYER && plr == NULL)
	{
		slog("Entity is not a player");
		return;
	}

	if (plr->flags & PLR_DEAD || plr->flags & PLR_INVIN)
	{
		return;
	}

	plr->health -= damage;
	slog("Took %i damage", damage);
	plr->flags = plr->flags | PLR_INVIN;
	plr->iframesRemaining = 80;
	if (plr->health <= 0)
	{
		plr->flags = PLR_DEAD;
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
	saveFile = fileName;
}


void load_player_inventory(Entity *ent, SJson *save)
{
	SJson *obj;
	int objFound;
	Player *player;
	Inventory *inv, *playerInv;

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
			if (inv == NULL)
			{
				slog("Failed to make inventory item");
				return;
			}
			inv->next = player->inventory;
			player->inventory = inv;
		}

		//Does that sword have a beam upgrade?
		obj = sj_object_get_value(save, "beamSword");
		if (obj != NULL)
		{
			sj_get_integer_value(obj, &objFound);
			if (objFound == 1)
			{
				player->inventory->flags = player->inventory->flags | INV_UPGRADED;
			}
		}
	}

	//Does the player have bombs?
	obj = sj_object_get_value(save, "bombs");
	if (obj != NULL)
	{
		inv = create_inventory("bombs");
		if (inv == NULL)
		{
			slog("Failed to make inventory item");
			return;
		}

		//Add bomb to back of player's inventory
		playerInv = player->inventory;
		if (playerInv == NULL)
		{
			player->inventory = inv;
		}
		else
		{
			while (playerInv->next != NULL)
			{
				playerInv = playerInv->next;
			}
			playerInv->next = inv;
		}
	}

	//Does the player have a gold key?
	obj = sj_object_get_value(save, "goldKey");
	if (obj != NULL)
	{
		inv = unlock_key("goldKey");
		if (inv == NULL)
		{
			slog("Failed to make inventory item");
			return;
		}

		//Add key to back of player's inventory
		playerInv = player->inventory;
		if (playerInv == NULL)
		{
			player->inventory = inv;
		}
		else
		{
			while (playerInv->next != NULL)
			{
				playerInv = playerInv->next;
			}
			playerInv->next = inv;
		}
	}
}

/*eol@eof*/
