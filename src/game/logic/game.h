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
#include "../interface/game_observer_hub.h"

class Game;
class Tank;
class Shot;
class Round;
class WeaponManager;

class Game : public GameView, public GameAdvancer, public GameObserverHub {
	const MazeGeneration maze_generation;
	const vector<Upgrade::Type> allowed_upgrades;
	unique_ptr<Round> round;
	int round_num;

	vector<Tank> tanks;

	void new_round();

	bool can_step() const;
	void step();
public:
	Game(
		MazeGeneration maze_generation,
		const set<Upgrade::Type> allowed_upgrades,
		int tank_num
	);

	PlayerInterface& get_player_interface(int player);

	int get_round() const;
	const Maze& get_maze() const;
	vector<TankCompleteState> get_states() const;
	vector<ShotPath> get_shots() const;
	vector<const ShrapnelState*> get_shrapnels() const;
	const set<unique_ptr<Upgrade>>& get_upgrades() const;

	void advance();
	void allow_step();

	void kill_tank(int index);
	void upgrade_tank(int index, Upgrade::Type type);
};

class WeaponManager{
public:
	virtual bool step(
		const TankState& owner_state,
		const KeyState& previous_keys,
		Round& round
	) = 0;
	virtual void reset() = 0;
};

class ShotManager : public WeaponManager, public GameObserver{
	Game& game;
	const int owner;
	set<int> shots;
public:
	ShotManager(int owner, Game& game);
	ShotManager(const ShotManager&) = delete;
	ShotManager(ShotManager&&) = delete;
	ShotManager& operator=(const ShotManager&) = delete;
	ShotManager& operator=(ShotManager&&) = delete;
	~ShotManager();

	bool step(
		const TankState& owner_state,
		const KeyState& previous_keys,
		Round& round
	);
	void reset();

	void on_shot_removed(int shot_id);
};


class AppliedUpgrade : public WeaponManager{
protected:
	TankUpgradeState state;
public:
	AppliedUpgrade(TankUpgradeState state);

	virtual bool allow_moving() const;
	const TankUpgradeState& get_state() const;
	virtual void reset();
};

class GatlingShotManager : public AppliedUpgrade{
	const int owner;
public:
	GatlingShotManager(int owner);
	
	bool step(
		const TankState& owner_state,
		const KeyState& previous_keys,
		Round& round
	);
};

class LaserManager : public AppliedUpgrade{
	const int owner;
public:
	LaserManager(int owner);
	
	bool step(
		const TankState& owner_state,
		const KeyState& previous_keys,
		Round& round
	);
};

class BombManager : public AppliedUpgrade, public GameObserver{
	Game& game;
	Round* round;
	const int owner;
	int shot;
public:
	BombManager(int owner, Game& game);
	BombManager(const BombManager&) = delete;
	BombManager(BombManager&&) = delete;
	BombManager& operator=(const BombManager&) = delete;
	BombManager& operator=(BombManager&&) = delete;
	~BombManager();
	
	bool step(
		const TankState& owner_state,
		const KeyState& previous_keys,
		Round& round
	);

	void on_shot_removed(int shot_id);
};

class Tank : public PlayerInterface{
	Game& game;
	const int index;

	TankState state;
	unique_ptr<ShotManager> shot_manager;
	unique_ptr<AppliedUpgrade> upgrade;
		
	deque<KeyState> pending_keys;
public:
	Tank(Game& game, int index);

	const TankState& get_state() const;
	const unique_ptr<AppliedUpgrade>& get_upgrade() const;
	void reset(int maze_w, int maze_h);
	void set_upgrade(Upgrade::Type type);

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
	vector<TimePoint> path;
	int ignored_tank;
protected:
	bool step(
		const Maze& maze, const vector<const TankState*>& tanks,
		vector<int>& killed_tanks
	);
public:
	Shot(ShotDetails&& details);

	const ShotDetails& get_state() const;
	const vector<TimePoint>& get_path() const;
};

class Shrapnel : public Projectile{
	ShrapnelState state;
protected:
	bool step(
		const Maze& maze, const vector<const TankState*>& tanks,
		vector<int>& killed_tanks
	);
public:
	Shrapnel(const ShrapnelDetails& details, const Maze& maze);
	
	const ShrapnelState& get_state() const;
};

class Round{
	Game& game;
	const vector<Upgrade::Type>& allowed_upgrades;

	int next_id;
	map<int, unique_ptr<Shot>> shots;
	set<int> removed_shots;
	set<unique_ptr<Shrapnel>> shrapnels;

	set<unique_ptr<Upgrade>> upgrades;
	int upgrade_timer;
	void create_upgrade();

	const Maze maze;
public:
	Round(Game& game, MazeGeneration maze_generation, const vector<Upgrade::Type>& allowed_upgrades);

	const Maze& get_maze() const;

	void step();

	int add_shot(unique_ptr<Shot>&& shot);
	void remove_shot(int shot_id);
	Shot* get_shot(int shot_id) const;
	const map<int, unique_ptr<Shot>>& get_shots() const;

	void explode(const Point& source);
	const set<unique_ptr<Shrapnel>>& get_shrapnels() const;
	
	const set<unique_ptr<Upgrade>>& get_upgrades() const;
};

#endif
