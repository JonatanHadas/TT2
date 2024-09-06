#include "geometry.h"

#include "../../utils/utils.h"

#define _USE_MATH_DEFINES
#include <math.h>

const Number WALL_WIDTH = Number(1) / 20;

#define TURN_NUM 72

const Number TURN_COS = cos(M_PI / (TURN_NUM / 2));
const Number TURN_SIN = sin(M_PI / (TURN_NUM / 2));

const Number TANK_WIDTH = Number(30) / 100;
const Number TANK_LENGTH = Number(45) / 100;

const Number TANK_SPEED = Number(3) / 100;
const Number TANK_REVERSE_SPEED = TANK_SPEED / 2;

void rotate(
	Number& direction_x, Number& direction_y,
	Number rotation_x, Number rotation_y
) {
	auto new_x = direction_x * rotation_x - direction_y * rotation_y;
	direction_y = direction_x * rotation_y + direction_y * rotation_x;
	direction_x = new_x;
}

void normalize(Number& x, Number& y){
	double length = sqrt((double)(x * x + y * y));
	x /= length;
	y /= length;
}

void random_direction(Number& direction_x, Number& direction_y){
	int direction = rand_range(0, TURN_NUM);

	direction_x = 1;
	direction_y = 0;
	
	Number cur_x = TURN_COS, cur_y = TURN_SIN;
	for(int mask = 1; mask < direction; mask <<= 1){
		if(mask & direction) rotate(
			direction_x, direction_y, 
			cur_x, cur_y
		);
		
		rotate(
			cur_x, cur_y,
			cur_x, cur_y
		);
	}
	
	normalize(direction_x, direction_y);
}
