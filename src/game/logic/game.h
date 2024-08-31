#ifndef _GAME_H
#define _GAME_H

#include <memory>

#include "../data/game_objects.h"
#include "../interface/game_view.h"

class Game;
class Round;

class Game : public GameView {
	const MazeGeneration maze_generation;
	unique_ptr<Round> round;

	void new_round();
public:
	Game(
		MazeGeneration maze_generation
	);
	
	const Maze& get_maze() const;
};

class Round{
	Game& game;
	
	const Maze maze;
public:
	Round(Game& game, MazeGeneration maze_generation);

	const Maze& get_maze() const;
};

#endif
