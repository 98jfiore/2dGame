#ifndef __SHAPES_H__
#define __SHAPES_H__

#include "SDL_stdinc.h"

#include "gfc_vector.h"

#define PI 3.14159265

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

/**
* @brief Get the angle between the x-axis and the line from origin to dest
* @param origin The first point
* @param dest The second point
* @return A double representing the angle found
*/
double PointsAngle(Vector2D origin, Vector2D dest);

/**
* @brief Get the angle between the x-axis and the line from origin to dest in radians
* @param origin The first point
* @param dest The second point
* @return A double representing the angle found
*/
double PointsAngle_Rad(Vector2D origin, Vector2D dest);

/**
* @brief Get the distance between two points
* @param origin The first point
* @param dest The second point
* @return A float representing the distance
*/
float PointsDistance(Vector2D origin, Vector2D dest);

/**
* @brief Get the midpoint between two points
* @param a The first point
* @param b The second point
* @return A vector representing the midpoint
*/
Vector2D GetMidpoint(Vector2D a, Vector2D b);

#endif