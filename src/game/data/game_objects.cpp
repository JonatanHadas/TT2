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

void KeyState::serialize(ostream& output) const{
	serialize_flags(output, left, right, forward, back, shoot);
}
KeyState KeyState::deserialize(istream& input){
	auto [left, right, forward, back, shoot] = deserialize_flags<5>(input);
	
	return KeyState(left, right, forward, back, shoot);
}

TankState::TankState(
	const Point& position,
	const Point& direction,
	const KeyState& key_state,
	bool active, bool alive
) :
	position(position),
	direction(direction),
	key_state(key_state),
	active(active),
	alive(alive) {
		
}

void TankState::serialize(ostream& output) const{
	serialize_value(output, position);
	serialize_value(output, direction);
	serialize_value(output, key_state);
	serialize_flags(output, active, alive);
}
TankState TankState::deserialize(istream& input){
	auto position = deserialize_value<Point>(input);
	auto direction = deserialize_value<Point>(input);
	auto key_state = deserialize_value<KeyState>(input);
	auto [active, alive] = deserialize_flags<2>(input);
	
	return TankState(
		position,
		direction,
		key_state,
		active, alive
	);
}

Upgrade::Upgrade(int x, int y, Type type) : x(x), y(y), type(type) {}

void Upgrade::serialize(ostream& output) const{
	serialize_value(output, x);
	serialize_value(output, y);
	serialize_value(output, (unsigned char)type);
}
Upgrade Upgrade::deserialize(istream& input){
	auto x = deserialize_value<int>(input);
	auto y = deserialize_value<int>(input);
	auto type = (Upgrade::Type)deserialize_value<unsigned char>(input);
	
	return Upgrade(x, y, type);
}

ShotDetails::ShotDetails(
	const Point& position,
	const Point& velocity,
	Number radius,
	int timer,
	Type type,
	int owner
) :
	position(position),
	velocity(velocity),
	radius(radius),
	timer(timer),
	type(type),
	owner(owner) {
		
}

void ShotDetails::serialize(ostream& output) const {
	serialize_value(output, position);
	serialize_value(output, velocity);
	serialize_value(output, radius);
	serialize_value(output, timer);
	serialize_value(output, owner);
	serialize_value(output, (unsigned char)(type));
}

ShotDetails ShotDetails::deserialize(istream& input){
	auto position = deserialize_value<Point>(input);
	auto velocity = deserialize_value<Point>(input);
	auto radius = deserialize_value<Number>(input);
	auto timer = deserialize_value<int>(input);
	auto owner = deserialize_value<int>(input);
	auto type = (ShotDetails::Type)deserialize_value<unsigned char>(input);
	
	return ShotDetails(
		position, velocity,
		radius,
		timer,
		type,
		owner
	);
}
