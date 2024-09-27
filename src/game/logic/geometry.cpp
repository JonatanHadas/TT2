#include "geometry.h"

#include "../../utils/utils.h"
#include "../../utils/serialization.h"

#include <deque>
#include <set>
#include <functional>

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

const Number UPGRADE_SIZE = Number(30) / 100;
const Point UPGRADE_ROTATION = {
	.x = cos(M_PI / 6),
	.y = Number(1) / 2,
};

const Number LASER_RADIUS = Number(1)/100;
const Number LASER_SPEED = Number(7)/10;
const int LASER_TTL = 20;


Point rotate(const Point& direction, const Point& rotation) {
	return {
		.x = direction.x * rotation.x - direction.y * rotation.y,
		.y = direction.x * rotation.y + direction.y * rotation.x
	};
}

double length(const Point& point){
	return sqrt((double)(point.x * point.x + point.y * point.y));
};

void normalize(Point& point){
	point /= length(point);
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
			if(first || current_depth < collision.depth){
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
			if(first || current_depth < collision.depth){
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

bool polygon_moving_circle_collision(
	const vector<Point>& polygon,
	const Point& position,
	const Point& velocity,
	Number radius,
	Point& normal,
	Number& fraction
) {
	
	Number max_fraction = 1, min_fraction = 0;
	
	for(int i = 0; i < polygon.size(); i++){
		Point edge = polygon[(i + 1) % polygon.size()] - polygon[i];
		normalize(edge);
		
		auto slope = cross(velocity, edge);
		auto intersect = (radius - cross(position - polygon[i], edge));
		
		if(slope == 0){
			if(intersect > 0) min_fraction = max_fraction + 1;
		}
		else{
			auto candidate = intersect / slope;
			if(slope > 0){
				if(candidate < max_fraction){
					max_fraction = candidate;
				}
			} else if(slope < 0){
				if(candidate > min_fraction){
					min_fraction = candidate;
					normal = { .x = edge.y, .y = -edge.x };
				}
			}
		}
		
		Point next_edge = polygon[(i + 2) % polygon.size()] - polygon[(i + 1) % polygon.size()];
		normalize(next_edge);
		
		edge += next_edge;
		Number distance = radius * length(edge);
		normalize(edge);
		
		slope = cross(velocity, edge);
		intersect = (distance - cross(position - polygon[(i + 1) % polygon.size()], edge));
		
		if(slope == 0){
			if(intersect > 0) min_fraction = max_fraction + 1;
		}
		else{
			auto candidate = intersect / slope;
			if(slope > 0){
				if(candidate < max_fraction){
					max_fraction = candidate;
				}
			} else {
				if(candidate > min_fraction){
					min_fraction = candidate;
					normal = { .x = edge.y, .y = -edge.x };
				}
			}
		}
	}
	
	fraction = min_fraction;
	bool collision = max_fraction > min_fraction;
	
	for(const auto& vertex: polygon){
		Point relative_position = position - vertex;
		Number slope = dot(relative_position, velocity);
		Number curve = dot(velocity, velocity);
		Number discriminant = slope.square() - curve * (dot(relative_position, relative_position) - radius.square());

		if((double)radius >= length(relative_position)){
			fraction = 0;
			collision = true;
			normal = relative_position;
			normalize(normal);
			break;  // Already inside
		}
		if(curve + radius < dot(relative_position, relative_position)){
			continue;  // To far
		}
		if(slope >= 0){
			continue;  // Going away
		}
		if(curve == 0) continue;
		
		if(discriminant < 0){
			continue;
		}
		
		Number lower = (-slope - sqrt((double)discriminant)) / curve;
		Number upper = (-slope + sqrt((double)discriminant)) / curve;
		
		if(lower < 0) lower = 0;
		if(upper > 0 && lower < (collision ? fraction : Number(1))){
			fraction = lower;
			collision = true;
			normal = position + velocity * fraction - vertex;
			normalize(normal);
		}
	}
	
	return collision;
}

Point simple_collision_displacement(const Collision& collision1, const Collision& collision2){
	return Point({
		.x = collision1.depth * collision2.normal.y - collision2.depth * collision1.normal.y,
		.y = collision2.depth * collision1.normal.x - collision1.depth * collision2.normal.x
	}) / cross(collision1.normal, collision2.normal);
}

bool get_collision_displacement(const vector<Collision>& collisions, Point& displacement){
	if(collisions.empty()) return false;

	const Collision* main = nullptr;
	set<const Collision*, function<bool(const Collision*, const Collision*)>> constraints([&](const Collision* collision1, const Collision* collision2){
		return dot(main->normal, collision1->normal) > dot(main->normal, collision2->normal);
	});
	for(const auto& collision: collisions){
		if(main == nullptr) main = &collision;
		else constraints.insert(&collision);
	}

	deque<const Collision*> left_collisions, right_collisions;
	deque<Point> left_points, right_points;
	bool using_main = true;
	for(auto collision: constraints){
		auto side = cross(main->normal, collision->normal);
		if(side == 0) return false;

		deque<const Collision*>& current = side > 0 ? right_collisions : left_collisions;
		deque<const Collision*>& other = side > 0 ? left_collisions : right_collisions;
		deque<Point>& current_points = side > 0 ? right_points : left_points;
		deque<Point>& other_points = side > 0 ? left_points : right_points;
		if(side > 0){
			if(left_collisions.size() > 0 && cross(left_collisions.back()->normal, collision->normal) <= 0) return false;
		}
		else{
			if(right_collisions.size() > 0 && cross(right_collisions.back()->normal, collision->normal) >= 0) return false;
		}

		while(current.size() > 0 && dot(collision->normal, current_points.back()) < collision->depth){
			current.pop_back();
			current_points.pop_back();
		}
		if(current.empty()){
			if(!using_main) other_points.pop_front();
			while(other_points.size() > 0 && dot(collision->normal, other_points.front()) < collision->depth){
				other_points.pop_front();
				if(using_main) using_main = false;
				else other.pop_front();
			}

			if(using_main){
				current_points.push_back(simple_collision_displacement(*main, *collision));
			}
			else{
				current_points.push_back(simple_collision_displacement(*other.front(), *collision));
				other_points.push_front(current_points.back());
			}
		}
		else{
			current_points.push_back(simple_collision_displacement(*current.back(), *collision));
		}
		current.push_back(collision);
	}

	if(!using_main) {
		main = left_collisions.front();
		left_collisions.pop_front();
		left_points.pop_front();
	}
	displacement = main->normal * main->depth;
	while(left_collisions.size() > 0 && dot(displacement, left_collisions.front()->normal) < left_collisions.front()->depth){
		displacement = left_collisions.front()->normal * left_collisions.front()->depth;
		if(dot(displacement, main->normal) < main->depth){
			displacement = left_points.front();
			return true;
		}
		main = left_collisions.front();
		left_collisions.pop_front();
		left_points.pop_front();
	}
	while(right_collisions.size() > 0 && dot(displacement, right_collisions.front()->normal) < right_collisions.front()->depth){
		displacement = right_collisions.front()->normal * right_collisions.front()->depth;
		if(dot(displacement, main->normal) < main->depth){
			displacement = right_points.front();
			return true;
		}
		main = right_collisions.front();
		right_collisions.pop_front();
		right_points.pop_front();
	}
	return true;
}

bool collision_rotate(const vector<Collision>& collisions, const Point& center, Point& direction, Number threshold){
	Point rotation = { .x = 1, .y = 0 };
	for(const auto& collision: collisions){
		auto candidate = -collision.depth / cross(collision.position - center, collision.normal);
		if(candidate > 0){
			if(rotation.y < 0 || candidate > threshold) return false;
			if(rotation.y < candidate) rotation.y = candidate;
		}
		if(candidate < 0){
			if(rotation.y > 0 || candidate < -threshold) return false;
			if(rotation.y > candidate) rotation.y = candidate;
		}
	}
	normalize(rotation);
	direction = rotate(direction, rotation);
	return true;
}