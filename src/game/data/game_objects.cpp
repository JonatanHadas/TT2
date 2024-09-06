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

KeyState::KeyState() : KeyState(false, false, false, false, false) {}
KeyState::KeyState(
	bool left,
	bool right,
	bool forward,
	bool back,
	bool shoot
) : left(left), right(right), forward(forward), back(back), shoot(shoot) {}

#define LEFT_MASK (1<<0)
#define RIGHT_MASK (1<<1)
#define FORWARD_MASK (1<<2)
#define BACK_MASK (1<<3)
#define SHOOT_MASK (1<<4)

void KeyState::serialize(ostream& output) const{
	unsigned char mask = 0;
	if(left) mask |= LEFT_MASK;
	if(right) mask |= RIGHT_MASK;
	if(forward) mask |= FORWARD_MASK;
	if(back) mask |= BACK_MASK;
	if(shoot) mask |= SHOOT_MASK;
	serialize_value(output, mask);
}
KeyState KeyState::deserialize(istream& input){
	auto mask = deserialize_value<unsigned char>(input);
	
	return KeyState(
		mask & LEFT_MASK,
		mask & RIGHT_MASK,
		mask & FORWARD_MASK,
		mask & BACK_MASK,
		mask & SHOOT_MASK
	);
}

TankState::TankState(
	Number x,
	Number y,
	Number direction_x,
	Number direction_y,
	KeyState key_state,
	bool active
) :
	x(x),
	y(y),
	direction_x(direction_x),
	direction_y(direction_y),
	key_state(key_state),
	active(active) {
		
}

void TankState::serialize(ostream& output) const{
	serialize_value(output, x);
	serialize_value(output, y);
	serialize_value(output, direction_x);
	serialize_value(output, direction_y);
	serialize_value(output, key_state);
	serialize_value(output, active);
}
TankState TankState::deserialize(istream& input){
	auto x = deserialize_value<Number>(input);
	auto y = deserialize_value<Number>(input);
	auto direction_x = deserialize_value<Number>(input);
	auto direction_y = deserialize_value<Number>(input);
	auto key_state = deserialize_value<KeyState>(input);
	auto active = deserialize_value<bool>(input);
	
	return TankState(
		x, y,
		direction_x, direction_y,
		key_state,
		active
	);
}
