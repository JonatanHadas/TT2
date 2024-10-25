#ifndef _MAZE_H
#define _MAZE_H

#include "../data/game_objects.h"

Maze generate_maze(MazeGeneration algorithm, int w, int h);

struct Direction{
	int dx, dy;
	int distance;
};

class MazeMap{
	vector<vector<vector<vector<Direction>>>> directions;
public:
	MazeMap(const Maze& maze);
	
	const Direction& get_direction(
		int start_x, int start_y,
		int end_x, int end_y
	) const;
};

#endif
