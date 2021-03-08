#include "simple_logger.h"

#include "font.h"
#include "gf2d_graphics.h"


typedef struct
{
	Font *font_list;
	Font *font_tags[FT_MAX];
	Uint32 font_max;
}FontManager;

static FontManager font_manager = { 0 };

void font_delete(Font *font)
{
	if (font == NULL) return;
	TTF_CloseFont(font->font);
	memset(font, 0, sizeof(Font));
}

void font_manager_close()
{
	int i;
	if (font_manager.font_list != NULL)
	{
		for (i = 0; i < font_manager.font_max; ++i)
		{
			font_delete(&font_manager.font_list[i]);
		}
	}
	free(font_manager.font_list);
	memset(&font_manager, 0, sizeof(FontManager));
}

void font_init(Uint32 maxFonts)
{
	if (!maxFonts)
	{
		slog("Cannot allocate 0 fonts");
		return;
	}
	if (!TTF_WasInit() && TTF_Init() == -1)
	{
		slog("TTF_Init: %s", TTF_GetError());
		exit(2);
	}
	font_manager_close();

	font_manager.font_max = maxFonts;

	font_manager.font_list = (Font *)gfc_allocate_array(sizeof(Font), maxFonts);

	atexit(font_manager_close);
}

Font *font_new()
{
	int i;
	for (i = 0; i < font_manager.font_max; ++i)
	{
		if (font_manager.font_list[i]._refCount == 0)
		{
			font_manager.font_list[i]._refCount = 1;
			return &font_manager.font_list[i];
		}
	}
	return NULL;
}

Font *font_get_by_file_point(const char *fileName, int ptsize)
{
	int i;
	for (i = 0; i < font_manager.font_max; ++i)
	{
		if (font_manager.font_list[i]._refCount == 0) continue;
		if (gfc_line_cmp(font_manager.font_list[i].filename, fileName) != 0) continue;
		if (font_manager.font_list[i].pointSize != ptsize) continue;
		return &font_manager.font_list[i];
	}
	return NULL;
}

Font *font_load(const char *filename, int ptsize)
{
	Font *fnt;

	fnt = font_get_by_file_point(filename, ptsize);
	if (fnt != NULL)
	{
		fnt->_refCount++;
		return fnt;
	}

	fnt = font_new();
	if (fnt == NULL)
	{
		slog("Could not allocate a font");
		return NULL;
	}

	fnt->pointSize = ptsize;
	gfc_line_cpy(fnt->filename,filename);

	fnt->font = TTF_OpenFont(filename, ptsize);
	if (fnt->font == NULL)
	{
		slog("Could not open font %s", filename);
		font_delete(fnt);
		return NULL;
	}

	return fnt;
}

void font_free(Font *font)
{
	if (font == NULL) return;
	font->_refCount--;
	if (font->_refCount == 0)
	{
		font_delete(font);
	}
}

void font_render(Font *font, char *text, Color color, Vector2D position)
{
	SDL_Surface *surface;
	SDL_Texture *texture;
	SDL_Rect target;

	if (!font)
	{
		slog("No font provided to render");
		return;
	}
	if (!text)
	{
		slog("No text provided to render");
		return;
	}
	surface = TTF_RenderText_Blended(font->font, text, gfc_color_to_sdl(color));
	if (!surface)
	{
		slog("Failed to render font text %s", text);
		return;
	}

	surface = gf2d_graphics_screen_convert(&surface);
	if (!surface)
	{
		slog("Failed to convert rendered text to graphics: %s", text);
		return;
	}


	texture = SDL_CreateTextureFromSurface(gf2d_graphics_get_renderer(), surface); 
	if (texture == NULL)
	{
		slog("Failed to convert rendered text to texture: %s", text);
		SDL_FreeSurface(surface);
		return;
	}

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_UpdateTexture(texture,
		NULL,
		surface->pixels,
		surface->pitch);

	target = gfc_sdl_rect(position.x,position.y,surface->w,surface->h);

	SDL_RenderCopyEx(gf2d_graphics_get_renderer(),
		texture,
		NULL,
		&target,
		0,
		NULL,
		0);

	/*if (frameHeight == -1)
	{
		sprite->frame_h = surface->h;
	}
	else sprite->frame_h = frameHeight;
	if (frameWidth == -1)
	{
		sprite->frame_w = surface->w;
	}
	else sprite->frame_w = frameWidth;
	sprite->frames_per_line = framesPerLine;
	gfc_line_cpy(sprite->filepath, filename);*/

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);

	//PAUSED: 38:54
}

/*eol@eof*/