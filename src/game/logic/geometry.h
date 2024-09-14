#ifndef _GAME_GEOMETRY_H
#define _GAME_GEOMETRY_H

#include "../../utils/numbers.h"

#include <vector>
#include <iostream>

using namespace std;

extern const Number WALL_WIDTH;

extern const Number TURN_COS;
extern const Number TURN_SIN;

extern const Number TANK_WIDTH;
extern const Number TANK_LENGTH;

extern const Number TANK_SPEED;
extern const Number TANK_REVERSE_SPEED;

Point rotate(const Point& direction, const Point& rotation);

double length(const Point& point);

void normalize(Point& point);

// random discrete direction
Point random_direction();

struct Collision{
	Point position;
	Point normal;
	Number depth;
};

/* Convex polygons only */
bool polygon_collision(
	const vector<Point>& polygon1,
	const vector<Point>& polygon2,
	Collision& collision
);

bool polygon_moving_circle_collision(
	const vector<Point>& polygon,
	const Point& position,
	const Point& velocity,
	Number radius,
	Point& normal,
	Number& fraction
);

bool get_collision_displacement(const vector<Collision>& collisions, Point& displacement);

bool collision_rotate(const vector<Collision>& collisions, const Point& center, Point& direction, Number threshold);

#endif