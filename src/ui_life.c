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

	if (life->_last == 1 && life->healthRep < player->maxhealth)
	{
		next = component_new();
		if (next == NULL)
		{
			slog("Failed to create component");
			return;
		}

		next->sprite = life->empty_sprite;
		if (next->sprite == NULL)
		{
			slog("UI sprite couldn't load");
			free(next);
			return;
		}

		next->position = vector2d(self->position.x + self->sprite->frame_w * self->scale.x + 2, self->position.y);
		next->frameCount = self->frameCount;
		next->baseFrame = self->baseFrame;
		next->scale = self->scale;
		next->frameRate = 0;

		next->free = health_component_free;
		next->update = health_component_update;

		nextLife = (UILife *)malloc(sizeof(UILife));
		nextLife->healthRep = life->healthRep + 1;
		nextLife->player = life->player;
		nextLife->full_sprite = life->full_sprite;
		nextLife->empty_sprite = life->empty_sprite;
		nextLife->_last = 1;
		next->data = nextLife;

		life->_last = 0;
	}

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

void gameOver_component_free(UIComponent *comp)
{
	TextUIComponent *text;
	UIGameOver *gameOver;

	if (comp == NULL) return;

	gameOver = (UIGameOver *)comp->data;
	if (gameOver == NULL) return;

	text = gameOver->text;
	font_free(text->font);
	free(text->text);
	free(text);

	free(gameOver);
}

void gameOver_component_update(UIComponent *self)
{
	UIGameOver *gameOver;

	if (self == NULL) return;

	gameOver = (UIGameOver *)self->data;
	if (gameOver == NULL) return;

	if (self->flags & UI_INVISIBLE)
	{
		if (gameOver->player == NULL || gameOver->player->health == 0)
		{
			self->flags = 0;
		}
	}
	else
	{
		if (gameOver->text->color.a < 255)
		{
			gameOver->text->color.a += 0.25;
		}
		if (gameOver->backdropColor.w < 175)
		{
			gameOver->backdropColor.w += 0.3;
		}
	}

}

void gameOver_component_draw(UIComponent *self)
{
	UIGameOver *gameOver;
	TextUIComponent *text;


	if (self->flags & UI_INVISIBLE) return;


	if (self == NULL) return;

	gameOver = (UIGameOver *)self->data;
	if (gameOver == NULL) return;

	if (self->sprite != NULL)
	{
		gf2d_sprite_draw(
			self->sprite,
			vector2d(0, 0),
			NULL,
			NULL,
			NULL,
			NULL,
			&gameOver->backdropColor,
			0);
	}

	text = gameOver->text;
	if (text == NULL) return;

	font_render(text->font, text->text, text->color, self->position);
}

UIComponent *gameOver_component_create(Entity *ent)
{
	UIComponent *comp;
	TextUIComponent *textComp;
	UIGameOver *gameOver;
	Player *player;

	if (ent->flags & ENT_PLAYER)
	{
		player = (Player *)ent->data;
		
		comp = component_new();
		if (comp == NULL)
		{
			slog("Failed to create component");
			return NULL;
		}

		comp->sprite = gf2d_sprite_load_image("images/backgrounds/void.png");
		if (comp->sprite == NULL)
		{
			slog("UI sprite couldn't load");
			component_free(comp);
			return NULL;
		}

		comp->frameCount = 0;
		comp->baseFrame = 0;
		comp->scale = vector2d(0, 0);
		comp->frameRate = 0;
		comp->flags = UI_INVISIBLE;

		gameOver = (UIGameOver *)malloc(sizeof(UIGameOver));
		if (gameOver == NULL)
		{
			slog("GAME OVER couldn't be allocated");
			component_free(comp);
			return NULL;
		}

		gameOver->player = player;

		textComp = (TextUIComponent *)malloc(sizeof(TextUIComponent));
		if (textComp == NULL)
		{
			slog("GAME OVER couldn't be allocated");
			free(gameOver);
			component_free(comp);
			return NULL;
		}

		textComp->font = font_load("fonts/RETRO_SPACE_INV.ttf", 70);
		textComp->color = gfc_color8(255,255,255,1);

		textComp->text = "GAME OVER";
		comp->position = vector2d(420, 300);

		gameOver->text = textComp;

		gameOver->backdropColor = vector4d(0, 0, 0, 10);

		comp->data = gameOver;
		comp->free = gameOver_component_free;
		comp->draw = gameOver_component_draw;
		comp->update = gameOver_component_update;

		return comp;
	}
	else
	{
		return NULL;
	}
}


/*eol@eof*/
