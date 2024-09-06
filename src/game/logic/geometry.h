#ifndef _GAME_GEOMETRY_H
#define _GAME_GEOMETRY_H

#include "../../utils/numbers.h"

extern const Number WALL_WIDTH;

extern const Number TURN_COS;
extern const Number TURN_SIN;

extern const Number TANK_WIDTH;
extern const Number TANK_LENGTH;

extern const Number TANK_SPEED;
extern const Number TANK_REVERSE_SPEED;

void rotate(
	Number& direction_x, Number& direction_y,
	Number rotation_x, Number rotation_y
);

void normalize(Number& x, Number& y);

// random discrete direction
void random_direction(Number& direction_x, Number& direction_y);

#endif