#ifndef _GAME_SETTINGS_H
#define _GAME_SETTINGS_H

#include <vector>
#include <iostream>

using namespace std;

class GameSettings{
public:
	GameSettings(
		const vector<int>& colors
	);

	vector<int> colors;

	void serialize(ostream& output) const;
	static GameSettings deserialize(istream& input);
};

#endif
