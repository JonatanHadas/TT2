#include "logic.h"

#include "geometry.h"

#include <vector>

using namespace std;

static inline vector<Point> get_maze_rect(int left, int right, int top, int bottom){
	return {
		{ .x = right + WALL_WIDTH, .y = top - WALL_WIDTH },
		{ .x = right + WALL_WIDTH, .y = bottom + WALL_WIDTH },
		{ .x = left - WALL_WIDTH, .y = bottom + WALL_WIDTH },
		{ .x = left - WALL_WIDTH, .y = top - WALL_WIDTH },
	};
}

vector<vector<Point>> get_maze_polygons(int x, int y, const Maze& maze){
	vector<vector<Point>> polygons;
	
	if(maze.has_hwall_below(x, y)){
		polygons.push_back(get_maze_rect(
			x - (maze.has_hwall_below(x - 1, y) ? 1 : 0),
			x + (maze.has_hwall_below(x + 1, y) ? 2 : 1),
			y + 1, y + 2
		));
	}
	if(maze.has_hwall_below(x, y - 1)){
		polygons.push_back(get_maze_rect(
			x - (maze.has_hwall_below(x - 1, y - 1) ? 1 : 0),
			x + (maze.has_hwall_below(x + 1, y - 1) ? 2 : 1),
			y - 1, y
		));
	}
	if(maze.has_vwall_right(x, y)){
		polygons.push_back(get_maze_rect(
			x + 1, x + 2,
			y - (maze.has_vwall_right(x, y - 1) ? 1 : 0),
			y + (maze.has_vwall_right(x, y + 1) ? 2 : 1)
		));
	}
	if(maze.has_vwall_right(x - 1, y)){
		polygons.push_back(get_maze_rect(
			x - 1, x,
			y - (maze.has_vwall_right(x - 1, y - 1) ? 1 : 0),
			y + (maze.has_vwall_right(x - 1, y + 1) ? 2 : 1)
		));
	}
	
	if(!(maze.has_hwall_below(x, y) || maze.has_vwall_right(x, y))){
		auto hwall = maze.has_hwall_below(x + 1, y), vwall = maze.has_vwall_right(x, y + 1);
		if(hwall || vwall) polygons.push_back(get_maze_rect(
			x + 1, x + (hwall ? 2 : 1),
			y + 1, y + (vwall ? 2 : 1)
		));
	}
	if(!(maze.has_hwall_below(x, y - 1) || maze.has_vwall_right(x, y))){
		auto hwall = maze.has_hwall_below(x + 1, y - 1), vwall = maze.has_vwall_right(x, y - 1);
		if(hwall || vwall) polygons.push_back(get_maze_rect(
			x + 1, x + (hwall ? 2 : 1),
			y - (vwall ? 1 : 0), y
		));
	}
	if(!(maze.has_hwall_below(x, y - 1) || maze.has_vwall_right(x - 1, y))){
		auto hwall = maze.has_hwall_below(x - 1, y - 1), vwall = maze.has_vwall_right(x - 1, y - 1);
		if(hwall || vwall) polygons.push_back(get_maze_rect(
			x - (hwall ? 1 : 0), x,
			y - (vwall ? 1 : 0), y
		));
	}
	if(!(maze.has_hwall_below(x, y) || maze.has_vwall_right(x - 1, y))){
		auto hwall = maze.has_hwall_below(x - 1, y), vwall = maze.has_vwall_right(x - 1, y + 1);
		if(hwall || vwall) polygons.push_back(get_maze_rect(
			x - (hwall ? 1 : 0), x,
			y + 1, y + (vwall ? 2 : 1)
		));
	}
	return polygons;
}

vector<Point> get_rotated_rectangle(
	const Point& center,
	const Point& direction, 
	Number width, Number length
){
	Point normal = { .x = direction.y, .y = -direction.x };
	return {
		center + ((direction * length) + (normal * width)) / 2,
		center + ((direction * length) - (normal * width)) / 2,
		center - ((direction * length) + (normal * width)) / 2,
		center - ((direction * length) - (normal * width)) / 2
	};
}

vector<Collision> get_tank_collisions(const TankState& tank, const Maze& maze){
	vector<Collision> collisions;
	
	const auto rect = get_rotated_rectangle(
		tank.position,
		tank.direction,
		TANK_WIDTH, TANK_LENGTH
	);

	for(const auto& wall: get_maze_polygons(tank.position.x, tank.position.y, maze)){
		Collision collision = {
			.position = { .x = 0, .y = 0 },
			.normal = { .x = 0, .y = 0 },
			.depth = 0
		};
		if(polygon_collision(rect, wall, collision)){
			collisions.push_back(collision);
		}
	}
	
	return collisions;
}

const Number EPSILON = Number(1) / 10000;

void advance_tank(TankState& tank, const Maze& maze){
	int turn_state = (tank.key_state.right ? 1 : 0) - (tank.key_state.left ? 1 : 0);

	if(turn_state){
		Point previous_direction = tank.direction;
		Point previous_position = tank.position;
		
		tank.direction = rotate(
			tank.direction,
			{ .x = TURN_COS, .y = turn_state * TURN_SIN }
		);
		normalize(tank.direction);

		auto collisions = get_tank_collisions(tank, maze);
		if(!collisions.empty()){
			Point displacement = { .x = 0, .y = 0 };
			for(auto& collision: collisions) collision.depth += EPSILON;

			if(get_collision_displacement(collisions, displacement)){
				tank.position -= displacement;
				collisions = get_tank_collisions(tank, maze);
			}
		}
		if(!collisions.empty()){
			tank.direction = previous_direction;
			tank.position = previous_position;
		}
	}
	
	
	Number speed = (
		tank.key_state.forward ? TANK_SPEED : Number(0)
	) - (
		tank.key_state.back ? TANK_REVERSE_SPEED : Number(0)
	);
	
	if(speed != 0){
		Point previous_direction = tank.direction;
		Point previous_position = tank.position;
		
		tank.position += tank.direction * speed;

		auto collisions = get_tank_collisions(tank, maze);
		if(!collisions.empty()){
			for(auto& collision: collisions) collision.depth += EPSILON;

			if(collision_rotate(collisions, tank.position, tank.direction, 2*TURN_SIN)){
				normalize(tank.direction);
				collisions = get_tank_collisions(tank, maze);
			}
		}
		if(!collisions.empty()){
			tank.direction = previous_direction;
			tank.position = previous_position;
		}
	}
}
