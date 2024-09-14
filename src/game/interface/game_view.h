#ifndef _GAME_VIEW_H
#define _GAME_VIEW_H

#include "../data/game_objects.h"

#include <vector>

using namespace std;

struct ShotPath{
	const ShotDetails& state;
	const vector<Point>& path;
};

class GameView{
public:
	virtual int get_round() const = 0;
	virtual const Maze& get_maze() const = 0;

	virtual vector<const TankState*> get_states() const = 0;
	virtual vector<ShotPath> get_shots() const = 0;
};

#endif
