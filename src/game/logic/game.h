#ifndef _GAME_H
#define _GAME_H

#include <memory>
#include <vector>
#include <deque>

#include "../../utils/numbers.h"

#include "../data/game_objects.h"
#include "../interface/game_view.h"
#include "../interface/game_advancer.h"
#include "../interface/player_interface.h"

class Game;
class Tank;
class Round;

class Game : public GameView, public GameAdvancer {
	const MazeGeneration maze_generation;
	unique_ptr<Round> round;
	int round_num;
	
	vector<Tank> tanks;

	void new_round();

	bool can_step() const;
	void step();
public:
	Game(
		MazeGeneration maze_generation,
		int tank_num
	);
	
	PlayerInterface& get_player_interface(int player);
	
	int get_round() const;
	const Maze& get_maze() const;
	vector<TankState> get_states() const;

	void advance();
	void allow_step();
};

class Tank : public PlayerInterface{
	Game& game;
	
	Number x, y, direction_x, direction_y;
	KeyState last_key_state;
	bool active;
	
	deque<KeyState> pending_keys;
public:
	Tank(Game& game);
	
	TankState get_state() const;
	void reset(int maze_w, int maze_h);
	
	void step(int round, KeyState key_state);
	void set_active(bool active);
	
	bool can_advance() const;
	void advance();
};

class Round{
	Game& game;
	
	const Maze maze;
public:
	Round(Game& game, MazeGeneration maze_generation);

	const Maze& get_maze() const;
};

#endif
