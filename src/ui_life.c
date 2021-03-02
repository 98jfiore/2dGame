#include "simple_logger.h"

#include "entity.h"
#include "player.h"
#include "ui.h"
#include "ui_life.h"

void health_component_free(UIComponent *comp)
{
	UILife *life;

	life = (UILife *)comp->data;

	gf2d_sprite_free(life->full_sprite);
	gf2d_sprite_free(life->empty_sprite);
}


UIComponent *health_component_create(const char *spriteFile, int sprite_w, int sprite_h, int sprite_fpl, int sprite_count, int sprite_num, const char *spriteFile_empty, int sprite_w_empty, int sprite_h_empty, int sprite_fpl_empty, int sprite_count_empty, int sprite_num_empty, int x, int y, int scale, Entity *ent)
{
	UIComponent *base;
	UILife *life;
	Player *player;
	int i;

	if (ent->flags & ENT_PLAYER)
	{
		player = (Player *)ent->data;
		for (i = 1; i <= player->maxhealth; ++i)
		{

			base = component_create(spriteFile, sprite_w, sprite_h, sprite_fpl, sprite_count, sprite_num, x + ((i-1) * (sprite_w * scale + 2)), y, scale);
			base->free = health_component_free;
			base->update = health_component_update;

			life = (UILife *)malloc(sizeof(UILife));
			life->healthRep = i;
			life->player = player;
			life->full_sprite = base->sprite;
			life->empty_sprite = gf2d_sprite_load_all(spriteFile_empty, sprite_w_empty, sprite_h_empty, sprite_fpl_empty);

			base->data = life;
			
			if (i == player->maxhealth)
			{
				life->_last = 1;
			}

			if (i > player->health)
			{
				base->sprite = life->empty_sprite;
			}
		}
	}

	return base;
}

void health_component_update(UIComponent *self)
{
	UILife *life, *nextLife;
	Player *player;
	UIComponent *next;
	
	life = (UILife *)self->data;
	player = life->player;

	/*if (life->_last && life->healthRep < player->maxhealth)
	{
		next = component_new();
		if (next == NULL)
		{
			slog("Failed to create component");
			return;
		}

		next->position = vector2d(self->position.x + self->sprite->frame_w * self->scale.x + 2, self->position.y);
		next->sprite = self->sprite;
		if (next->sprite == NULL)
		{
			slog("UI sprite couldn't load");
			free(next);
			return;
		}
		next->frameCount = self->frameCount;
		next->baseFrame = self->baseFrame;
		next->scale = self->scale;
		next->frameRate = 0;

		next->free = health_component_free;
		next->update = health_component_update;

		nextLife = (UILife *)malloc(sizeof(UILife));
		nextLife->healthRep = life->healthRep + 1;
		nextLife->ent = life->ent;
		nextLife->full_sprite = life->full_sprite;
		nextLife->empty_sprite = life->empty_sprite;
		nextLife->_last = 1;
		life->_last = 0;
	}*/

	if (life->healthRep > player->health)
	{
		if (self->sprite == life->full_sprite)
		{
			self->sprite = life->empty_sprite;
		}
	}
	else
	{
		if (self->sprite == life->empty_sprite)
		{
			self->sprite = life->full_sprite;
		}
	}
}

/*eol@eof*/
