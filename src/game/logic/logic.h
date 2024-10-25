#ifndef _GAME_LOGIC_H
#define _GAME_LOGIC_H

#include "../data/game_objects.h"

void advance_tank(TankState& tank, const Maze& maze);

int advance_shot(
	ShotDetails& shot,
	const Maze& maze,
	const vector<const TankState*>& tanks,
	int& ignored_tank,
	vector<TimePoint>& collisions
);

void advance_missile(
	MissileDetails& missile,
	int turn_direction,
	const Maze& maze
);
bool check_missile_tank_collision(
	const MissileDetails& missile,
	const TankState& tank
);

Number get_shrapnel_wall_collision(const ShrapnelDetails& shrapnel, const Maze& maze);
Number get_shrapnel_tank_collision(const ShrapnelDetails& shrapnel, const TankState& tank);
Number get_shrapnel_way(int time);

bool check_upgrade_collision(const TankState& tank, const Upgrade& upgrade);

#endif
