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

extern const Number UPGRADE_SIZE;
extern const Point UPGRADE_ROTATION;


extern const Number LASER_RADIUS;
extern const Number LASER_SPEED;
extern const int LASER_TTL;

extern const int SHRAPNEL_TTL;

extern const Number MISSILE_WIDTH;
extern const Number MISSILE_LENGTH;
extern const Number MISSILE_LAUNCHER_LENGTH;

extern const Number MISSILE_TURN_COS;
extern const Number MISSILE_TURN_SIN;

extern const Number MISSILE_SPEED;

extern const Number MINE_SIZE;
extern const Number MINE_DISTANCE;
extern const int MINE_START_TIME;

extern const Number DEATH_RAY_WIDTH;
extern const int DEATH_RAY_LOAD_TIME;


Point rotate(const Point& direction, const Point& rotation);

double length(const Point& point);

void normalize(Point& point);

Number dot(const Point& point1, const Point& point2);
Number cross(const Point& point1, const Point& point2);

// random discrete direction
Point random_discrete_direction();

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