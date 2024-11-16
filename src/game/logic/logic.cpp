#include "logic.h"

#include "geometry.h"

#include <vector>
#include <math.h>

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

int advance_shot(
	ShotDetails& shot,
	const Maze& maze,
	const vector<const TankState*>& tanks,
	int& ignored_tank,
	vector<TimePoint>& collisions
){	
	collisions.push_back({
		.point = shot.position,
		.time = 1
	});

	Point remaining_way = shot.velocity;

	int tank_collision = -1;

	bool finished = false;

	while(!finished){
		Point step = remaining_way;
		auto len = length(step);
		if(len < 0.5){
			finished = true;
		} else {
			if( len > 1 ) normalize(step);
			step /= 2;
		}
		
		Number fraction = 1;
		Point normal = { .x = 0, .y = 0 };
		for(const auto& polygon: get_maze_polygons(shot.position.x, shot.position.y, maze)){
			Number current_fraction = 0;
			Point current_normal = { .x = 0, .y = 0 };
			
			if(polygon_moving_circle_collision(
				polygon,
				shot.position, step,
				shot.radius,
				current_normal, current_fraction
			)){
				if(current_fraction < fraction && (current_normal.x * shot.velocity.x < 0 || current_normal.y * shot.velocity.y < 0)){
					ignored_tank = -1;
					finished = false;
					fraction = current_fraction;
					normal = current_normal;
				}
			}
		}
		
		for(int tank_index = 0; tank_index < tanks.size(); tank_index++){
			if(!tanks[tank_index]->alive) continue;

			auto polygon = get_rotated_rectangle(
				tanks[tank_index]->position,
				tanks[tank_index]->direction,
				TANK_WIDTH, TANK_LENGTH
			);

			Number current_fraction = 0;
			Point current_normal = { .x = 0, .y = 0 };
			
			if(polygon_moving_circle_collision(
				polygon,
				shot.position, step,
				shot.radius,
				current_normal, current_fraction
			)){
				if(tank_index != ignored_tank && current_fraction < fraction){
					finished = true;
					tank_collision = tank_index;
					fraction = current_fraction;
				}
			}
		}
		
		step *= fraction;
		shot.position += step;
		remaining_way -= step;
		
		if(tank_collision < 0 && fraction < 1){  // Wall collision
			if(normal.x * shot.velocity.x < 0){
				shot.velocity.x = -shot.velocity.x;
				remaining_way.x = -remaining_way.x;
			}
			if(normal.y * shot.velocity.y < 0){
				shot.velocity.y = -shot.velocity.y;
				remaining_way.y = -remaining_way.y;
			}
			collisions.push_back({
				.point = shot.position,
				.time = length(remaining_way) / length(shot.velocity),
			});
		}
	}

	return tank_collision;
}

void advance_missile(
	MissileDetails& missile,
	int turn_direction,
	const Maze& maze
){
	if(turn_direction){
		missile.direction = rotate(
			missile.direction,
			{ .x = MISSILE_TURN_COS, .y = turn_direction * MISSILE_TURN_SIN }
		);
		normalize(missile.direction);
	}
	
	missile.position += missile.direction * MISSILE_SPEED;
	
	const auto rect = get_rotated_rectangle(
		missile.position,
		missile.direction,
		MISSILE_WIDTH, MISSILE_LENGTH
	);

	for(const auto& wall: get_maze_polygons(missile.position.x, missile.position.y, maze)){
		Collision collision = {
			.position = { .x = 0, .y = 0 },
			.normal = { .x = 0, .y = 0 },
			.depth = 0
		};
		if(polygon_collision(rect, wall, collision)){
			if(collision.normal.x * missile.direction.x > 0) missile.direction.x = -missile.direction.x;
			if(collision.normal.y * missile.direction.y > 0) missile.direction.y = -missile.direction.y;
		}
	}
}
bool check_missile_tank_collision(
	const MissileDetails& missile,
	const TankState& tank
){
	Collision collision = {
		.position = { .x = 0, .y = 0 },
		.normal = { .x = 0, .y = 0 },
		.depth = 0
	};
	return tank.alive && polygon_collision(
		get_rotated_rectangle(
			missile.position,
			missile.direction,
			MISSILE_WIDTH, MISSILE_LENGTH
		),
		get_rotated_rectangle(
			tank.position,
			tank.direction,
			TANK_WIDTH, TANK_LENGTH
		),
		collision
	);
}

const Number TURN_THRESHOLD = Number(1)/20;

int target_missile_turn(
	const MazeMap& maze_map,
	const MissileDetails& missile,
	const vector<const TankState*>& tanks,
	int& target
){
	target = -1;
	Direction direction = {
		.dx = 0, .dy = 0,
		.distance = -1
	};
	
	for(int i = 0; i < tanks.size(); i++){
		if(!tanks[i]->alive) continue;
		auto current_direction = maze_map.get_direction(
			missile.position.x, missile.position.y,
			tanks[i]->position.x, tanks[i]->position.y
		);
		
		if(
			target == -1 || 
			current_direction.distance < direction.distance || (			
				current_direction.distance == 0 &&
				length(tanks[i]->position - missile.position) < length(tanks[target]->position - missile.position)
			)
		){
			direction = current_direction;
			target = i;
		}
	}
	
	if(target == -1) {
		return 0;
	}
	
	Point target_position = tanks[target]->position;
	if(direction.distance > 0){
		target_position = { .x = 1, .y = 1 };
		target_position /= 2;
		target_position.x += (int)missile.position.x + direction.dx;
		target_position.y += (int)missile.position.y + direction.dy;
	}
	
	Point target_direction = target_position - missile.position;
	normalize(target_direction);
	Number turn_direction = cross(missile.direction, target_direction);	
	
	if(turn_direction > TURN_THRESHOLD){
		return 1;
	}
	if(turn_direction < -TURN_THRESHOLD){
		return -1;
	}
	return 0;
}

Number get_shrapnel_wall_collision(const ShrapnelDetails& shrapnel, const Maze& maze){
	int sections = 1 + 2 * length(shrapnel.distance);
	Point position = shrapnel.start;
	Point step = shrapnel.distance / sections;
	for(int i = 0; i < sections; i++, position += step){
		bool collision = false;
		Number fraction = 0;
		for(const auto& polygon: get_maze_polygons(position.x, position.y, maze)){
			Number current_fraction = 0;
			Point current_normal = { .x = 0, .y = 0 };
			
			if(polygon_moving_circle_collision(
				polygon,
				position, step,
				0,
				current_normal, current_fraction
			)){
				if(!collision || current_fraction < fraction){
					fraction = current_fraction;
					collision = true;
				}
			}
		}
		
		if(collision) return (fraction + i) / sections;
	}
	return 2;
}
Number get_shrapnel_tank_collision(const ShrapnelDetails& shrapnel, const TankState& tank){
	if(!tank.alive) return 2;

	auto polygon = get_rotated_rectangle(
		tank.position,
		tank.direction,
		TANK_WIDTH, TANK_LENGTH
	);
	
	Number fraction = 0;
	Point normal = { .x = 0, .y = 0 };
	if(polygon_moving_circle_collision(
		polygon,
		shrapnel.start, shrapnel.distance,
		0,
		normal, fraction
	)) return fraction;
	
	return 2;
}

Number get_shrapnel_way(int time){
	if(time > SHRAPNEL_TTL) return 1;
	return 1.0 - pow(1.0 - (time / (double)SHRAPNEL_TTL), 2.6);
}

bool check_upgrade_collision(const TankState& tank, const Upgrade& upgrade){
	Collision collision = { .position = { .x = 0, .y = 0 }, .normal = { .x = 0, .y = 0 }, .depth = 0 };
	return polygon_collision(
		get_rotated_rectangle(
			tank.position, tank.direction,
			TANK_WIDTH, TANK_LENGTH
		),
		get_rotated_rectangle(
			{ .x = Number(2 * upgrade.x + 1) / 2, .y = Number(2 * upgrade.y + 1) / 2 },
			UPGRADE_ROTATION, UPGRADE_SIZE, UPGRADE_SIZE
		),
		collision
	);
}

static inline vector<Point> get_mine_polygon(const Point& position, const Point& direction){
	static const Point mine_vertices[2] = {
		{ .x = (MINE_SIZE * 4) / 5, .y = -MINE_SIZE / 5 },
		{ .x = (MINE_SIZE * 4) / 5, .y = MINE_SIZE / 5 },
	};
	static const Point mine_rotations[3] = {
		{ .x = 1, .y = 0 },
		{ .x = -Number(1) / 2, .y = sqrt(3)/2 },
		{ .x = -Number(1) / 2, .y = -sqrt(3)/2 },
	};

	vector<Point> polygon;
	for(const Point& rotation: mine_rotations){
		Point total_rotation = rotate(rotation, direction);
		for(const Point& vertex: mine_vertices){
			normalize(total_rotation);
			polygon.push_back(position + rotate(vertex, total_rotation));
		}
	}
	return polygon;
}

bool check_mine_collision(const MineDetails& mine, const TankState& tank){
	Collision collision = { .position = { .x = 0, .y = 0 }, .normal = { .x = 0, .y = 0 }, .depth = 0 };
		
	return polygon_collision(
		get_rotated_rectangle(
			tank.position, tank.direction,
			TANK_WIDTH, TANK_LENGTH
		),
		get_mine_polygon(mine.position, mine.direction),
		collision
	);
}