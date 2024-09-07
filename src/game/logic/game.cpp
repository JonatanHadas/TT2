#include "game.h"

#include "../../utils/utils.h"

#include "geometry.h"
#include "logic.h"
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
	state(
		{ .x = -1, .y = -1 } /*position*/,
		{ .x = 1, .y = 0 } /*direction*/,
		KeyState(),
		true /*active*/
	) {
	
}

const TankState& Tank::get_state() const{
	return state;
}
void Tank::reset(int maze_w, int maze_h){
	state.position.x = Number(2 * rand_range(0, maze_w) + 1) / 2;
	state.position.y = Number(2 * rand_range(0, maze_h) + 1) / 2;
	
	state.direction = random_direction();
	state.key_state = KeyState();
	pending_keys.clear();
}

void Tank::step(int round, KeyState key_state){
	if(round == game.get_round()) pending_keys.push_back(key_state);
}
void Tank::set_active(bool active){
	state.active = active;
	pending_keys.clear();
}

bool Tank::can_advance() const{
	return !state.active || !pending_keys.empty();
}
void Tank::advance(){
	if(state.active){
		state.key_state = pending_keys.front();
		pending_keys.pop_front();
	}
	else{
		state.key_state = KeyState();
	}

	advance_tank(state, game.get_maze());
}
