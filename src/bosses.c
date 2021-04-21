#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "bosses.h"
#include "player.h"
#include "ent_npc.h"
#include "ent_bomb.h"
#include "shapes.h"

static char *playerFile = "saves/save.json";

Entity *boss1_spawn(Vector2D position)
{
	Entity *ent;
	Entity *subent;
	Vector2D subent_pos;
	Boss_One *boss;
	Rect *hitbox;
	Vector2D velocity;
	const char *name = "boss_one";

	ent = npc_spawn(position);
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->sprite = gf2d_sprite_load_all("images/boss1_base_sprites.png", 32, 32, 1);
	ent->baseFrame = 0;
	ent->frame = 0;
	ent->frameCount = 1;
	ent->frameRate = 0.05;
	ent->damage = 1;
	ent->scale = vector2d(2, 2);
	ent->flags = ENT_DEADLY;

	ent->update = boss1_update;
	ent->free = boss1_free;

	hitbox = (Rect *)malloc(sizeof(Rect));

	hitbox->x = position.x - 1;
	hitbox->y = position.y - 1;
	hitbox->width = ent->sprite->frame_w * 2 - 2;
	hitbox->height = ent->sprite->frame_h * 2 - 2;
	ent->hitbox = hitbox;

	boss = (Boss_One *)malloc(sizeof(Boss_One));
	boss->num_subents = 4;
	boss->subents = (Entity **)gfc_allocate_array(sizeof(Entity *), boss->num_subents);
	
	//Set up subents
	vector2d_copy(subent_pos, position);
	subent_pos.x = subent_pos.x - (8 * 2);
	subent_pos.y = subent_pos.y + (24 * 2);

	subent = entity_new();
	if (subent == NULL)
	{
		slog("Could not make subent");
		entity_free(ent);
		return NULL;
	}
	subent->position = subent_pos;

	subent->sprite = gf2d_sprite_load_all("images/boss1_sprites.png", 16, 16, 3);
	subent->frameCount = 1;
	subent->baseFrame = 0;
	subent->frame = 0;
	subent->frameRate = 0.05;
	subent->scale = vector2d(2, 2);
	subent->flags = 0;
	boss->subents[1] = subent;

	vector2d_copy(subent_pos, position);
	subent_pos.x = subent_pos.x - (15 * 2);
	subent_pos.y = subent_pos.y + (35 * 2);

	subent = entity_new();
	if (subent == NULL)
	{
		slog("Could not make subent");
		entity_free(ent);
		return NULL;
	}
	subent->position = subent_pos;

	subent->sprite = gf2d_sprite_load_all("images/boss1_sprites.png", 16, 16, 3);
	subent->frameCount = 2;
	subent->baseFrame = 1;
	subent->frame = 1;
	subent->frameRate = 0.05;
	subent->scale = vector2d(2, 2);
	subent->damage = 1;
	subent->health = 3;
	subent->flags = ENT_DEADLY |  ENT_HITTABLE | ENT_HASHEALTH;

	hitbox = (Rect *)malloc(sizeof(Rect));
	hitbox->x = subent_pos.x - 1;
	hitbox->y = subent_pos.y - 1;
	hitbox->width = subent->sprite->frame_w * 2 - 2;
	hitbox->height = subent->sprite->frame_h * 2 - 2;
	subent->hitbox = hitbox;

	boss->subents[0] = subent;


	vector2d_copy(subent_pos, position);





	subent = entity_new();
	if (subent == NULL)
	{
		slog("Could not make subent");
		entity_free(ent);
		return NULL;
	}

	subent->sprite = gf2d_sprite_load_all("images/boss1_sprites.png", 16, 16, 3);
	subent_pos.x = subent_pos.x + ent->sprite->frame_w * 2 - (8 * 2);
	subent_pos.y = subent_pos.y + (24 * 2);
	subent->position = subent_pos;
	subent->frameCount = 1;
	subent->baseFrame = 0;
	subent->frame = 0;
	subent->frameRate = 0.05;
	subent->scale = vector2d(2, 2);
	subent->flags = 0;
	boss->subents[3] = subent;



	vector2d_copy(subent_pos, position);
	
	subent = entity_new();
	if (subent == NULL)
	{
		slog("Could not make subent");
		entity_free(ent);
		return NULL;
	}

	subent->sprite = gf2d_sprite_load_all("images/boss1_sprites.png", 16, 16, 3);
	subent_pos.x = subent_pos.x + ent->sprite->frame_w * 2;
	subent_pos.y = subent_pos.y + (35 * 2);
	subent->position = subent_pos;
	subent->frameCount = 2;
	subent->baseFrame = 1;
	subent->frame = 1;
	subent->frameRate = 0.05;
	subent->scale = vector2d(2, 2);
	subent->damage = 1;
	subent->health = 3;
	subent->flags = ENT_DEADLY | ENT_HITTABLE | ENT_HASHEALTH;

	hitbox = (Rect *)malloc(sizeof(Rect));
	hitbox->x = subent_pos.x - 1;
	hitbox->y = subent_pos.y - 1;
	hitbox->width = subent->sprite->frame_w * 2 - 2;
	hitbox->height = subent->sprite->frame_h * 2 - 2;
	subent->hitbox = hitbox;

	boss->subents[2] = subent;

	boss->tag = (char *)malloc(strlen(name) * sizeof(char));
	strcpy(boss->tag, name);

	boss->player = get_player();

	velocity = vector2d(0, 0);

	ent->velocity = velocity;
	ent->damage = 1;

	ent->data = boss;

	return ent;
}

void boss1_update(Entity *self)
{
	Boss_One *boss;
	Vector2D pos;

	boss = (Boss_One *)self->data;
	if (boss == NULL)
	{
		return;
	}

	if (boss->subents[0] == NULL && boss->subents[2] == NULL)
	{
		save_playerUpgrade(playerFile, boss->tag);
		vector2d_copy(pos, self->position);
		pos.x = pos.x - self->scale.x * (self->sprite->frame_w / 2);
		pos.y = pos.y - self->scale.y * (self->sprite->frame_h / 2);
		entity_free(self);
		explosion_spawn(pos, NULL);
	}
	else
	{
		if (boss->subents[0] != NULL)
		{
			if (boss->subents[0]->health <= 0)
			{
				entity_free(boss->subents[0]);
				boss->subents[0] = NULL;
			}
		}
		if (boss->subents[2] != NULL)
		{
			if (boss->subents[2]->health <= 0)
			{
				entity_free(boss->subents[2]);
				boss->subents[2] = NULL;
			}
		}
	}
}

void boss1_free(Entity *self)
{
	Boss_One *boss;
	int i;

	boss = (Boss_One *)self->data;
	if (boss == NULL)
	{
		return;
	}
	for (i = 0; i < boss->num_subents; ++i)
	{
		if (boss->subents[i] == NULL) continue;
		entity_free(boss->subents[i]);
	}
	free(self->data);
}


/*eol@eof*/