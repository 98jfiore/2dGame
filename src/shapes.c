#include <math.h>

#include "shapes.h"

SDL_bool IntersectRect(Rect *a, Rect *b)
{
	if (a->x >= b->x && a->x <= b->x + b->width && a->y >= b->y && a->y <= b->y + b->height)
	{
		return SDL_TRUE;
	}
	if (a->x + a->width >= b->x && a->x + a->width <= b->x + b->width && a->y >= b->y && a->y <= b->y + b->height)
	{
		return SDL_TRUE;
	}
	if (a->x >= b->x && a->x <= b->x + b->width && a->y + a->height >= b->y && a->y + a->height <= b->y + b->height)
	{
		return SDL_TRUE;
	}
	if (a->x + a->width >= b->x && a->x + a->width <= b->x + b->width && a->y + a->height >= b->y && a->y + a->height <= b->y + b->height)
	{
		return SDL_TRUE;
	}
	if (b->x >= a->x && b->x <= a->x + a->width && b->y >= a->y && b->y <= a->y + a->height)
	{
		return SDL_TRUE;
	}
	if (b->x >= a->x && b->x <= a->x + a->width && b->y + b->height >= a->y && b->y + b->height <= a->y + a->height)
	{
		return SDL_TRUE;
	}
	if (b->x + b->width >= a->x && b->x + b->width <= a->x + a->width && b->y >= a->y && b->y <= a->y + a->height)
	{
		return SDL_TRUE;
	}
	if (b->x + b->width >= a->x && b->x + b->width <= a->x + a->width && b->y + b->height >= a->y && b->y + b->height <= a->y + a->height)
	{
		return SDL_TRUE;
	}
	return SDL_FALSE;
}


double PointsAngle(Vector2D origin, Vector2D dest)
{
	double ydiff, xdiff, baseAngle;

	ydiff = fabs(dest.y - origin.y);
	xdiff = fabs(origin.x - dest.x);

	baseAngle = atan(ydiff / xdiff) * 180.0 / PI;

	if (dest.y < origin.y)
	{
		if (dest.x > origin.x)
		{
			return baseAngle;
		}
		else
		{
			return (180-baseAngle);
		}
	}
	else
	{
		if (dest.x < origin.x)
		{
			return baseAngle + 180;
		}
		else
		{
			return (360 - baseAngle);
		}
	}
}

double PointsAngle_Rad(Vector2D origin, Vector2D dest)
{
	return PointsAngle(origin, dest) * PI / 180;
}

float PointsDistance(Vector2D origin, Vector2D dest)
{
	return sqrtf(powf(origin.x - dest.x, 2) + powf(origin.y - dest.y, 2));
}

Vector2D GetMidpoint(Vector2D a, Vector2D b)
{
	return vector2d(fabs(a.x - b.x) / 2 + min(a.x, b.x), fabs(a.y - b.y) / 2 + min(a.y, b.y));
}


