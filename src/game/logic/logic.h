#ifndef _GAME_LOGIC_H
#define _GAME_LOGIC_H

#include "../data/game_objects.h"

void advance_tank(TankState& tank, const Maze& maze);

int advance_shot(
	ShotDetails& shot,
	const Maze& maze,
	const vector<const TankState*>& tanks,
	int& ignored_tank,
	vector<Point>& collisions
);

#endif
