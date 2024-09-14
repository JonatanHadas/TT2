#ifndef _GAME_H
#define _GAME_H

#include <memory>
#include <vector>
#include <set>
#include <map>
#include <deque>

#include "../../utils/numbers.h"

#include "../data/game_objects.h"
#include "../interface/game_view.h"
#include "../interface/game_advancer.h"
#include "../interface/player_interface.h"

class Game;
class Tank;
class Shot;
class Round;
class WeaponManager;

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
	vector<const TankState*> get_states() const;
	vector<ShotPath> get_shots() const;

	void advance();
	void allow_step();

	void kill_tank(int index);
};

class WeaponManager{
public:
	virtual bool step(const TankState& owner_state, Round& round) = 0;
	virtual void reset() = 0;
};

class ShotManager : public WeaponManager{
	const int owner;
	bool pressed;
	set<int> shots;
public:
	ShotManager(int owner);

	bool step(const TankState& owner_state, Round& round);
	void reset();
};

class Tank : public PlayerInterface{
	Game& game;
	const int index;

	TankState state;
	ShotManager shot_manager;
		
	deque<KeyState> pending_keys;
public:
	Tank(Game& game, int index);

	const TankState& get_state() const;
	void reset(int maze_w, int maze_h);

	void step(int round, KeyState key_state);
	void set_active(bool active);

	bool can_advance() const;
	void advance(Round& round);

	void kill();
};

class Projectile{
protected:
	virtual bool step(
		const Maze& maze, const vector<const TankState*>& tanks,
		vector<int>& killed_tanks
	) = 0;
public:
	bool advance(Game& game);
};

class Shot : public Projectile{
	ShotDetails state;
	vector<Point> path;
	int ignored_tank;
protected:
	bool step(
		const Maze& maze, const vector<const TankState*>& tanks,
		vector<int>& killed_tanks
	);
public:
	Shot(ShotDetails&& details, int shooter);

	const ShotDetails& get_state() const;
	const vector<Point>& get_path() const;
};

class Round{
	Game& game;

	int next_id;
	map<int, unique_ptr<Shot>> shots;

	const Maze maze;
public:
	Round(Game& game, MazeGeneration maze_generation);

	const Maze& get_maze() const;

	void step();

	int add_shot(unique_ptr<Shot>&& shot);
	void remove_shot(int shot_id);
	Shot* get_shot(int shot_id) const;
	const map<int, unique_ptr<Shot>>& get_shots() const;
};

#endif
