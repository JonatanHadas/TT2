#include "game_settings.h"

#include "../../utils/serialization.h"

GameSettings::GameSettings(
	const vector<int>& colors
) :
	colors(colors) {
		
}

void GameSettings::serialize(ostream& output) const{
	serialize_value(output, colors);
}
GameSettings GameSettings::deserialize(istream& input){
	auto colors = deserialize_value<vector<int>>(input);
	return GameSettings(
		colors
	);
}
