#include "geometry.h"

#include "../../utils/utils.h"
#include "../../utils/serialization.h"

#include <deque>
#include <algorithm>

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

Point rotate(const Point& direction, const Point& rotation) {
	return {
		.x = direction.x * rotation.x - direction.y * rotation.y,
		.y = direction.x * rotation.y + direction.y * rotation.x
	};
}

void normalize(Point& point){
	point /= sqrt((double)(point.x * point.x + point.y * point.y));
}

Point random_direction(){
	int direction = rand_range(0, TURN_NUM);

	Point result{
		.x = 1,
		.y = 0
	};
	
	Point cur {.x = TURN_COS, .y = TURN_SIN};
	for(int mask = 1; mask < direction; mask <<= 1){
		if(mask & direction) result = rotate(result, cur);
		
		cur = rotate(cur, cur);
	}
	
	normalize(result);
	return result;
}

static inline Number dot(const Point& point1, const Point& point2){
	return point1.x * point2.x + point1.y * point2.y;
}

static inline Number cross(const Point& point1, const Point& point2){
	return point1.x * point2.y - point1.y * point2.x;
}

#include <iostream>

bool polygon_collision(
	const vector<Point>& polygon1,
	const vector<Point>& polygon2,
	Collision& collision
){
	auto edge1 = polygon1[1] - polygon1[0];
	normalize(edge1);
	
	int starting_index2 = polygon2.size();
	while(cross(
		edge1,
		polygon2[(starting_index2 + 1) % polygon2.size()] - polygon2[starting_index2 % polygon2.size()]
	) > 0) starting_index2 ++;  // next polygon2 edge is going closer
	while(cross(
		edge1,
		polygon2[(starting_index2 - 1) % polygon2.size()] - polygon2[starting_index2 % polygon2.size()]
	) > 0) starting_index2 --;  // previous polygon2 edge is going farther
	
	bool first = true;
	auto edge2 = polygon2[(starting_index2 + 1) % polygon2.size()] - polygon2[starting_index2 % polygon2.size()];	

	normalize(edge2);
	for(
		int index1 = 0, index2 = starting_index2;
		index1 < polygon1.size() || index2 < starting_index2 + polygon2.size();
	){
		if(cross(edge1, edge2) > 0){
			Number current_depth = cross(
				edge2,
				polygon1[index1 % polygon1.size()] - polygon2[index2 % polygon2.size()]
			);
			if(current_depth < 0) return false;
			if(first || current_depth > collision.depth){
				first = false;
				collision.depth = current_depth;
				collision.normal = {
					.x = -edge2.y,
					.y = edge2.x
				};
				collision.position = polygon1[index1 % polygon1.size()];
			}
			index2 += 1;
			
			edge2 = polygon2[(index2 + 1) % polygon2.size()] - polygon2[index2 % polygon2.size()];
			normalize(edge2);
		} else {
			Number current_depth = cross(
				edge1,
				polygon2[index2 % polygon1.size()] - polygon1[index1 % polygon1.size()]
			);
			if(current_depth < 0) return false;
			if(first || current_depth > collision.depth){
				first = false;
				collision.depth = current_depth;
				collision.normal = {
					.x = edge1.y,
					.y = -edge1.x
				};
				collision.position = polygon2[index2 % polygon1.size()];
			}
			index1 += 1;
			
			edge1 = polygon1[(index1 + 1) % polygon1.size()] - polygon1[index1 % polygon1.size()];
			normalize(edge1);
		}
	}
	
	return true;
}
