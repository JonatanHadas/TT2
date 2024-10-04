#ifndef _GAME_VIEW_H
#define _GAME_VIEW_H

#include "../data/game_objects.h"

#include <vector>
#include <set>
#include <memory>

using namespace std;

struct ShotPath{
	int id;
	const ShotDetails& state;
	const vector<TimePoint>& path;
};

struct TankUpgradeState{
	const Upgrade::Type type;
	int state;
	int timer;
};

struct TankCompleteState{
	const TankState& state;
	const TankUpgradeState* upgrade;
};

struct ShrapnelState{
	const ShrapnelDetails details;
	const Number collision;
	int timer;
};

class GameView{
public:
	virtual int get_round() const = 0;
	virtual const Maze& get_maze() const = 0;

	virtual vector<TankCompleteState> get_states() const = 0;
	virtual vector<ShotPath> get_shots() const = 0;
	virtual vector<const ShrapnelState*> get_shrapnels() const = 0;
	virtual const set<unique_ptr<Upgrade>>& get_upgrades() const = 0;
};

#endif
