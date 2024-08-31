#include "game_objects.h"

#include "../../utils/serialization.h"

Maze::Maze(vector<vector<bool>>&& hwalls, vector<vector<bool>>&& vwalls) :
	hwalls(hwalls),
	vwalls(vwalls) {

}

int Maze::get_w() const {
	return hwalls.size();
}
int Maze::get_h() const{
	return vwalls.at(0).size();
}

bool Maze::has_hwall_below(int x, int y) const{
	if(x < 0 || x >= get_w() || y < 0 || y >= get_h() - 1) return true;
	return hwalls.at(x).at(y);
}
bool Maze::has_vwall_right(int x, int y) const{
	if(x < 0 || x >= get_w() - 1 || y < 0 || y >= get_h()) return true;
	return vwalls.at(x).at(y);
}

void Maze::serialize(ostream& output) const{
	serialize_value(output, hwalls);
	serialize_value(output, vwalls);
}
Maze Maze::deserialize(istream& input) {
	auto hwalls = deserialize_value<vector<vector<bool>>>(input);
	auto vwalls = deserialize_value<vector<vector<bool>>>(input);
	
	return Maze(std::move(hwalls), std::move(vwalls));
}
