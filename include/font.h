#ifndef __FONT_H__
#define __FONT_H__

#include <string.h>
#include <SDL_ttf.h>

#include "gfc_color.h"
#include "gfc_text.h"
#include "gfc_types.h"

typedef struct
{
	Uint32 _refCount;
	TextLine filename;
	TTF_Font *font;
	Uint32  pointSize;
}Font;


/**
* @brief Close the font manager
* @param font A pointer to the font we want to delete
*/
void font_delete(Font *font);

/**
* @brief Close the font manager
*/
void font_manager_close();

/**
* @brief Initialize the font manager
* @param maxFonts How many concurrent fonts to support
*/
void font_init(Uint32 maxFonts);

/**
* @brief Load a font from a file
* @param filename The path to the desired ttf file
* @param ptsize The size of the font you want to load
* @return Null on error, otherwise a pointer to the loaded font
*/
Font *font_load(const char *filename, int ptsize);

/**
* @brief Free a font
* @param font A pointer to the font we want to free
*/
void font_free(Font *font);

/**
* @brief Render text on screen with a font
* @param font A pointer to the font we want to render the text with
* @param text The text you want to render
* @param color The color of the text you want to render
* @param position Where on screen you want text to render
*/
void font_render(Font *font, char *text, Color color, Vector2D position);

#endif
