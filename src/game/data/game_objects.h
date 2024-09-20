#ifndef _GAME_OBJECTS_H
#define _GAME_OBJECTS_H

#include "../../utils/numbers.h"

#include <ostream>
#include <istream>
#include <vector>

using namespace std;

enum class MazeGeneration : unsigned char{
	NONE = 0,
	EXPAND_TREE = 1
};

class Maze{
	vector<vector<bool>> hwalls, vwalls;
public:
	Maze(vector<vector<bool>>&& hwalls, vector<vector<bool>>&& vwalls);
	
	int get_w() const;
	int get_h() const;
	
	bool has_hwall_below(int x, int y) const;
	bool has_vwall_right(int x, int y) const;
	
	void serialize(ostream& output) const;
	static Maze deserialize(istream& input);
};

class KeyState{
public:
	KeyState();
	KeyState(
		bool left,
		bool right,
		bool forward,
		bool back,
		bool shoot
	);

	bool left, right, forward, back, shoot;
	
	void serialize(ostream& output) const;
	static KeyState deserialize(istream& input);
};

class TankState{
public:
	TankState(
		const Point& position,
		const Point& direction,
		const KeyState& key_state,
		bool active, bool alive
	);

	Point position, direction;
	KeyState key_state;
	bool active, alive;
	
	void serialize(ostream& output) const;
	static TankState deserialize(istream& input);
};

class Upgrade {
public:
	enum class Type : unsigned char{
		GATLING = 0
	} type;
	int x, y;
	
	Upgrade(int x, int y, Type type);

	void serialize(ostream& output) const;
	static Upgrade deserialize(istream& input);
};

class ShotDetails{
public:
	ShotDetails(
		const Point& position,
		const Point& velocity,
		Number radius,
		int timer
	);
	
	Point position, velocity;
	Number radius;
	int timer;
	
	void serialize(ostream& output) const;
	static ShotDetails deserialize(istream& input);
};

#endif
