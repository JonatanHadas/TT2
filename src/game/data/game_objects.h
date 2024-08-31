#ifndef _GAME_OBJECTS_H
#define _GAME_OBJECTS_H

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

#endif
