#ifndef __SHAPES_H__
#define __SHAPES_H__

#include "SDL_stdinc.h"

typedef struct
{
	float x;
	float y;
	float width;
	float height;
} Rect;

/**
* @brief See if the two rectangles ever intersect
* @param a The first rectangle
* @param b The second rectangle
* @return An SDL boolean variable representing whether or not the rectangles intersected.
*/
SDL_bool IntersectRect(Rect *a, Rect *b);

#endif