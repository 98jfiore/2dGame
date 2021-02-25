#include "shapes.h"

SDL_bool IntersectRect(Rect *a, Rect *b)
{
	if (a->x > b->x && a->x < b->x + b->width && a->y > b->y && a->y < b->y + b->height)
	{
		return SDL_TRUE;
	}
	if (a->x + a->width > b->x && a->x + a->width < b->x + b->width && a->y > b->y && a->y < b->y + b->height)
	{
		return SDL_TRUE;
	}
	if (a->x > b->x && a->x < b->x + b->width && a->y + a->height > b->y && a->y + a->height < b->y + b->height)
	{
		return SDL_TRUE;
	}
	if (a->x + a->width > b->x && a->x + a->width < b->x + b->width && a->y + a->height > b->y && a->y + a->height < b->y + b->height)
	{
		return SDL_TRUE;
	}
	return SDL_FALSE;
}