#include "game.h"

#include "../../utils/utils.h"

#include "geometry.h"
#include "maze.h"

Game::Game(
	MazeGeneration maze_generation,
	int tank_num
) :
	maze_generation(maze_generation),
	tanks(),
	round_num(-1) {
		
	for(int i = 0; i < tank_num; i++){
		tanks.push_back(Tank(*this));
	}
		
	new_round();
}

void Game::new_round(){
	round_num += 1;
	
	round = make_unique<Round>(*this, maze_generation);
	
	for(auto& tank: tanks){
		tank.reset(round->get_maze().get_w(), round->get_maze().get_h());
	}
};

#include <iostream>

bool Game::can_step() const{
	for(const auto& tank: tanks){
		if(!tank.can_advance()) return false;
	}
	return true;
}
void Game::step(){
	for(auto& tank: tanks){
		tank.advance();
	}
}

PlayerInterface& Game::get_player_interface(int player){
	return tanks[player];
}


int Game::get_round() const{
	return round_num;
}

const Maze& Game::get_maze() const{
	return round->get_maze();
}
vector<TankState> Game::get_states() const{
	vector<TankState> states;
	for(const auto& tank: tanks){
		states.push_back(tank.get_state());
	}
	return states;
}


void Game::advance(){
	while(can_step()) step();
}

void Game::allow_step(){}

Round::Round(Game& game, MazeGeneration maze_generation) :
	game(game),
	maze(generate_maze(maze_generation, rand_range(5, 12), rand_range(5, 12))) {

}

const Maze& Round::get_maze() const{
	return maze;
}

Tank::Tank(Game& game) :
	game(game),
	x(-1),
	y(-1),
	direction_x(1),
	direction_y(0),
	active(true) {
	
}

TankState Tank::get_state() const{
	return TankState(
		x, y,
		direction_x, direction_y,
		last_key_state,
		active
	);
}
void Tank::reset(int maze_w, int maze_h){
	x = Number(2 * rand_range(0, maze_w) + 1) / 2;
	y = Number(2 * rand_range(0, maze_h) + 1) / 2;
	
	random_direction(direction_x, direction_y);
	last_key_state = KeyState();
	pending_keys.clear();
}

void Tank::step(int round, KeyState key_state){
	if(round == game.get_round()) pending_keys.push_back(key_state);
}
void Tank::set_active(bool active){
	this->active = active;
	pending_keys.clear();
}

bool Tank::can_advance() const{
	return !active || !pending_keys.empty();
}
void Tank::advance(){
	if(active){
		last_key_state = pending_keys.front();
		pending_keys.pop_front();
	}
	else{
		last_key_state = KeyState();
	}
	
	int turn_state = (last_key_state.right ? 1 : 0) - (last_key_state.left ? 1 : 0);
	if(turn_state){
		rotate(
			direction_x, direction_y,
			TURN_COS, turn_state * TURN_SIN
		);
		normalize(direction_x, direction_y);
	}
	
	Number speed = (
		last_key_state.forward ? TANK_SPEED : Number(0)
	) - (
		last_key_state.back ? TANK_REVERSE_SPEED : Number(0)
	);
	
	x += direction_x * speed;
	y += direction_y * speed;
}
