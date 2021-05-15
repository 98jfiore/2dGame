#include <math.h>

#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "bosses.h"
#include "player.h"
#include "ent_npc.h"
#include "entity.h"
#include "ent_bomb.h"
#include "shapes.h"

static char *playerFile = "saves/save.json";
int head_maxCycle = 180;
int head_health = 4;

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

	hitbox->x = position.x + 1;
	hitbox->y = position.y + 1;
	hitbox->width = ent->sprite->frame_w * 2 - 2;
	hitbox->height = ent->sprite->frame_h * 2 - 2;
	ent->hitbox = hitbox;

	boss = (Boss_One *)malloc(sizeof(Boss_One));
	boss->num_subents = 4;
	boss->subents = (Entity **)gfc_allocate_array(sizeof(Entity *), boss->num_subents);
	boss->flags = MOV_NORTH;
	boss->cycle = 60;
	boss->head1_cycle = 70;
	boss->head2_cycle = 90;
	boss->chase_head = 3;
	
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
	subent->health = head_health;
	subent->flags = ENT_DEADLY |  ENT_HITTABLE | ENT_HASHEALTH;

	hitbox = (Rect *)malloc(sizeof(Rect));
	hitbox->x = subent_pos.x + 1;
	hitbox->y = subent_pos.y + 1;
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
	subent->health = head_health;
	subent->flags = ENT_DEADLY | ENT_HITTABLE | ENT_HASHEALTH;

	hitbox = (Rect *)malloc(sizeof(Rect));
	hitbox->x = subent_pos.x + 1;
	hitbox->y = subent_pos.y + 1;
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


	/*boss->ps = particle_source_new(12000, 0, 25, 600, 50, vector2d(200, 300),
		vector2d(150, 5), vector2d(0, 20), vector2d(0, 1), vector2d(0.2, 0.2), vector2d(0.1, 0.1),
		gfc_color(1, 1, 1, 255), gfc_color(0, 0, 0, -0.1), gfc_color(1, 1, 1, 0.1),
		"images/oil.png", 8, 8, 1, 3, 41, 0.1, 0.05, SDL_BLENDMODE_ADD, PART_MODE_B, 0.01, 0.001, 0.2, 0.4, 360, 0.0005, 0.2, 0.3);*/
	boss->ps = NULL;
	ent->draw = boss1_draw;

	return ent;
}

void boss1_update(Entity *self)
{
	Boss_One *boss;
	Entity *exp;
	Vector2D mov, pa, pb, playerpos, line;
	float dist;
	int i;

	boss = (Boss_One *)self->data;
	if (boss == NULL)
	{
		return;
	}

	if (boss->player == NULL || boss->player->_inuse == 0) return;

	if (boss->subents[0] == NULL && boss->subents[2] == NULL)
	{
		save_playerUpgrade(playerFile, boss->tag);
		exp = explosion_spawn(self->position, NULL);
		exp->hitbox->x = self->position.x - self->scale.x * (8);
		exp->hitbox->y = self->position.y - self->scale.y * (8);
		entity_free(self);
		return;
	}
	else
	{
		if (boss->subents[0] != NULL)
		{
			if (boss->head1_cycle > 0) boss->head1_cycle--;
			if (boss->head1_cycle == 0)
			{
				boss->head1_cycle = head_maxCycle;
				if (boss->chase_head & 1) boss->head1_cycle = boss->head1_cycle / 2;
				boss->chase_head = boss->chase_head ^ 1;
			}

			if (boss->subents[0]->health <= 0)
			{
				entity_free(boss->subents[0]);
				boss->head2_cycle = -1;
				boss->chase_head = 2;
				boss->subents[0] = NULL;
				if (boss->subents[3] != NULL)
				{
					boss->ps = particle_source_new(10000, 0, 5, 600, 50, boss->subents[1]->position,
						vector2d(20, 20), vector2d(0, 20), vector2d(0, 1), vector2d(0.2, 0.2), vector2d(0.1, 0.1),
						gfc_color(1, 1, 1, 254), gfc_color(0.01, 0.01, 0.01, -0.04), gfc_color(1, 1, 1, 1),
						"images/oil.png", 8, 8, 1, 3, 41, 0.1, 0.05, SDL_BLENDMODE_ADD, PART_MODE_B, 0.01, 0, 0.1, 0, 10, 0.01, 0, 3);
				}
			}
			else
			{
				pa = vector2d(boss->subents[0]->position.x + (boss->subents[0]->sprite->frame_w * boss->subents[0]->scale.x) / 2, boss->subents[0]->position.y + (boss->subents[0]->sprite->frame_h * boss->subents[0]->scale.y) / 2);
				pb = vector2d(self->position.x - (7 * 2), self->position.y + (27 * 2));

				if (boss->player != NULL && boss->chase_head & 1)
				{
					playerpos = vector2d(boss->player->position.x + (boss->player->sprite->frame_w * boss->player->scale.x) / 2, boss->player->position.y + (boss->player->sprite->frame_h * boss->player->scale.y) / 2);
					line = vector2d(playerpos.x - pa.x, playerpos.y - pa.y);
					dist = PointsDistance(playerpos, pa);
					mov = vector2d(1.3 * line.x / dist, 1.3 * line.y / dist);

					vector2d_add(boss->subents[0]->position, boss->subents[0]->position, mov);
					pa = vector2d(boss->subents[0]->position.x + (boss->subents[0]->sprite->frame_w * boss->subents[0]->scale.x) / 2, boss->subents[0]->position.y + (boss->subents[0]->sprite->frame_h * boss->subents[0]->scale.y) / 2);
					dist = PointsDistance(pa, pb);
					if (dist > 170)
					{
						line = vector2d(pb.x - pa.x, pb.y - pa.y);
						dist = PointsDistance(pb, pa);
						mov = vector2d(((dist - 90)/dist) * line.x / dist, ((dist - 90)/dist) * line.y / dist);
						vector2d_add(boss->subents[0]->position, boss->subents[0]->position, mov);
					}
					else if (dist < 30)
					{
						line = vector2d(pb.x - pa.x, pb.y - pa.y);
						dist = PointsDistance(pb, pa);
						mov = vector2d(((dist - 40) / dist) * line.x / dist, ((dist - 40) / dist) * line.y / dist);
						vector2d_add(boss->subents[0]->position, boss->subents[0]->position, mov);
					}
				}
				else
				{
					line = vector2d(pb.x - pa.x, pb.y - pa.y);
					dist = PointsDistance(pb, pa);
					if (dist > 30)
					{
						mov = vector2d(0.8 * line.x / dist, 0.8 * line.y / dist);

						vector2d_add(boss->subents[0]->position, boss->subents[0]->position, mov);
					}
				}
				boss->subents[1]->position = GetMidpoint(boss->subents[0]->position, vector2d(self->position.x + (self->sprite->frame_w * self->scale.x / 2) - (boss->subents[1]->sprite->frame_w * boss->subents[1]->scale.x / 2), self->position.y + (self->sprite->frame_h * self->scale.y / 2) - (boss->subents[1]->sprite->frame_h * boss->subents[1]->scale.y / 2)));
			}
		}
		if (boss->subents[2] != NULL)
		{
			if (boss->head2_cycle > 0) boss->head2_cycle--;
			if (boss->head2_cycle == 0)
			{
				boss->head2_cycle = head_maxCycle;
				if (boss->chase_head & 2) boss->head2_cycle = boss->head2_cycle / 2;
				boss->chase_head = boss->chase_head ^ 2;
			}

			if (boss->subents[2]->health <= 0)
			{
				entity_free(boss->subents[2]);
				boss->head1_cycle = -1;
				boss->chase_head = 1;
				boss->subents[2] = NULL;
				if (boss->subents[0] != NULL)
				{
					boss->ps = particle_source_new(15000, 0, 5, 600, 50, boss->subents[3]->position,
						vector2d(20, 20), vector2d(0, 20), vector2d(0, 1), vector2d(0.2, 0.2), vector2d(0.1, 0.1),
						gfc_color(1, 1, 1, 254), gfc_color(0.01, 0.01, 0.01, -0.04), gfc_color(1, 1, 1, 1),
						"images/oil.png", 8, 8, 1, 3, 41, 0.1, 0.05, SDL_BLENDMODE_ADD, PART_MODE_B, 0.01, 0, 0.1, 0, 10, 0.01, 0, 3);
				}
			}
			else
			{
				if (boss->player != NULL && boss->chase_head & 2)
				{
					pa = vector2d(boss->subents[2]->position.x + (boss->subents[2]->sprite->frame_w * boss->subents[2]->scale.x) / 2, boss->subents[2]->position.y + (boss->subents[2]->sprite->frame_h * boss->subents[2]->scale.y) / 2);
					pb = vector2d(self->position.x + self->sprite->frame_w * 2, self->position.y + (27 * 2));
					playerpos = vector2d(boss->player->position.x + (boss->player->sprite->frame_w * boss->player->scale.x) / 2, boss->player->position.y + (boss->player->sprite->frame_h * boss->player->scale.y) / 2);
					line = vector2d(playerpos.x - pa.x, playerpos.y - pa.y);
					dist = PointsDistance(playerpos, pa);
					mov = vector2d(1.3 * line.x / dist, 1.3 * line.y / dist);

					vector2d_add(boss->subents[2]->position, boss->subents[2]->position, mov);
					pa = vector2d(boss->subents[2]->position.x + (boss->subents[2]->sprite->frame_w * boss->subents[2]->scale.x) / 2, boss->subents[2]->position.y + (boss->subents[2]->sprite->frame_h * boss->subents[2]->scale.y) / 2);
					dist = PointsDistance(pa, pb);
					if (dist > 170)
					{
						line = vector2d(pb.x - pa.x, pb.y - pa.y);
						dist = PointsDistance(pb, pa);
						mov = vector2d(((dist - 90) / dist) * line.x / dist, ((dist - 90) / dist) * line.y / dist);
						vector2d_add(boss->subents[2]->position, boss->subents[2]->position, mov);
					}
					else if (dist < 30)
					{
						line = vector2d(pb.x - pa.x, pb.y - pa.y);
						dist = PointsDistance(pb, pa);
						mov = vector2d(((dist - 40) / dist) * line.x / dist, ((dist - 40) / dist) * line.y / dist);
						vector2d_add(boss->subents[2]->position, boss->subents[2]->position, mov);
					}
				}
				else
				{
					pa = vector2d(boss->subents[2]->position.x + (boss->subents[2]->sprite->frame_w * boss->subents[2]->scale.x) / 2, boss->subents[2]->position.y + (boss->subents[2]->sprite->frame_h * boss->subents[2]->scale.y) / 2);
					pb = vector2d(self->position.x + self->sprite->frame_w * 2, self->position.y + (27 * 2)); 
					line = vector2d(pb.x - pa.x, pb.y - pa.y);
					dist = PointsDistance(pb, pa);
					if (dist > 40)
					{
						mov = vector2d(0.8 * line.x / dist, 0.8 * line.y / dist);

						vector2d_add(boss->subents[2]->position, boss->subents[2]->position, mov);
					}
				}
				boss->subents[3]->position = GetMidpoint(boss->subents[2]->position, vector2d(self->position.x + (self->sprite->frame_w * self->scale.x / 2) - (boss->subents[3]->sprite->frame_w * boss->subents[3]->scale.x / 2), self->position.y + (self->sprite->frame_h * self->scale.y / 2) - (boss->subents[3]->sprite->frame_h * boss->subents[3]->scale.y / 2)));
			}
		}
	}

	if (boss->flags & MOV_NORTH)
	{
		mov = vector2d(-0.4, 0);
	}
	else if (boss->flags & MOV_EAST)
	{
		mov = vector2d(0, 0.4);
	}
	else if (boss->flags & MOV_SOUTH)
	{
		mov = vector2d(0.4, 0);
	}
	else
	{
		mov = vector2d(0, -0.4);
	}

	vector2d_add(self->position, self->position, mov);
	for (i = 0; i < 4; i++)
	{
		if (boss->subents[i] != NULL)
		{
			vector2d_add(boss->subents[i]->position, boss->subents[i]->position, mov);
		}
	}

	boss->cycle--;
	if (boss->cycle <= 0)
	{
		if (boss->flags & MOV_NORTH)
		{
			boss->flags = MOV_EAST;
		}
		else if (boss->flags & MOV_EAST)
		{
			boss->flags = MOV_SOUTH;
		}
		else if (boss->flags & MOV_SOUTH)
		{
			boss->flags = MOV_WEST;
		}
		else
		{
			boss->flags = MOV_NORTH;
		}

		boss->cycle = 120;
	}

	boss1_updateHitboxes(self);

	if (boss->ps != NULL)
	{
		if (boss->subents[0] == NULL)
		{
			mov = vector2d(boss->subents[1]->position.x + boss->subents[1]->sprite->frame_w * boss->subents[1]->scale.x / 2, boss->subents[1]->position.y + boss->subents[1]->sprite->frame_h * boss->subents[1]->scale.y / 2);
			vector2d_copy(boss->ps->position, mov);
		}
		else if (boss->subents[2] == NULL)
		{
			mov = vector2d(boss->subents[3]->position.x + boss->subents[3]->sprite->frame_w * boss->subents[3]->scale.x / 2, boss->subents[3]->position.y + boss->subents[3]->sprite->frame_h * boss->subents[3]->scale.y / 2);
			vector2d_copy(boss->ps->position, mov);
		}
		particle_source_update(boss->ps);
	}
}

void boss1_draw(Entity *self)
{
	Vector2D upperLeft;
	Boss_One *boss;

	if (self == NULL)
	{
		slog("CAnnot draw a NULL entity");
		return;
	}

	boss = (Boss_One *)self->data;
	if (boss == NULL)
	{
		return;
	}

	if (self->sprite == NULL)
	{
		return; //Nothing to draw
	}
	if (boss->ps != NULL)
	{
		particle_source_draw(boss->ps);
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
	if (boss->ps != NULL)
	{
		particle_source_draw(boss->ps);
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
	if (boss->ps != NULL)
	{
		particle_source_free(boss->ps);
	}
	for (i = 0; i < boss->num_subents; ++i)
	{
		if (boss->subents[i] == NULL) continue;
		entity_free(boss->subents[i]);
	}
	free(self->data);
}

void boss1_updateHitboxes(Entity *self)
{
	Boss_One *boss;

	boss = (Boss_One *)self->data;
	if (boss == NULL)
	{
		return;
	}
	
	self->hitbox->x = self->position.x + 1;
	self->hitbox->y = self->position.y + 1;
	if (boss->subents[0] != NULL)
	{
		boss->subents[0]->hitbox->x = boss->subents[0]->position.x + 1;
		boss->subents[0]->hitbox->y = boss->subents[0]->position.y + 1;
	}
	if (boss->subents[2] != NULL)
	{
		boss->subents[2]->hitbox->x = boss->subents[2]->position.x + 1;
		boss->subents[2]->hitbox->y = boss->subents[2]->position.y + 1;
	}
}


/*eol@eof*/