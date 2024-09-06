#ifndef _GAME_VIEW_H
#define _GAME_VIEW_H

#include "../data/game_objects.h"

#include <vector>

using namespace std;

class GameView{
public:
	virtual int get_round() const = 0;
	virtual const Maze& get_maze() const = 0;

	virtual vector<TankState> get_states() const = 0;
};

#endif
