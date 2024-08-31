#ifndef _GAME_VIEW_H
#define _GAME_VIEW_H

#include "../data/game_objects.h"

class GameView{
public:
	virtual const Maze& get_maze() const = 0;
};

#endif
