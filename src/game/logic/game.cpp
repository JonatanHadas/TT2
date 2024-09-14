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
		tanks.push_back(Tank(*this, i));
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

bool Game::can_step() const{
	for(const auto& tank: tanks){
		if(!tank.can_advance()) return false;
	}
	return true;
}
void Game::step(){
	for(auto& tank: tanks){
		tank.advance(*round);
	}
	round->step();
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
vector<const TankState*> Game::get_states() const{
	vector<const TankState*> states;
	for(const auto& tank: tanks){
		states.push_back(&tank.get_state());
	}
	return states;
}
vector<ShotPath> Game::get_shots() const{
	vector<ShotPath> shots;
	for(const auto& entry: round->get_shots()){
		shots.push_back(ShotPath({
			.state=entry.second->get_state(),
			.path=entry.second->get_path()
		}))	;
	}
	
	return shots;
}

void Game::advance(){
	while(can_step()) step();
}

void Game::allow_step(){}

Round::Round(Game& game, MazeGeneration maze_generation) :
	game(game),
	next_id(0),
	maze(generate_maze(maze_generation, rand_range(5, 12), rand_range(5, 12))) {

}

const Maze& Round::get_maze() const{
	return maze;
}

int Round::add_shot(unique_ptr<Shot>&& shot){
	int shot_id = next_id++;
	shots.insert({shot_id, move(shot)});
	return shot_id;
}
void Round::remove_shot(int shot_id){
	shots.erase(shot_id);
}
Shot* Round::get_shot(int shot_id) const{
	auto it = shots.find(shot_id);
	if(it == shots.end()) return nullptr;
	return it->second.get();
}
const map<int, unique_ptr<Shot>>& Round::get_shots() const{
	return shots;
}

void Round::step(){
	
	vector<int> removed_ids;
	for(const auto& shot_entry: shots){
		if(shot_entry.second->advance(game)){
			removed_ids.push_back(shot_entry.first);
		}
	}
	for(int shot_id: removed_ids){
		remove_shot(shot_id);
	}
}

const Number CANNON_LENGTH = Number(17)/100;

const Number SHOT_RADIUS = Number(3)/100;
const Number SHOT_SPEED = Number(1)/25;
const int SHOT_TTL = 1200;
const int MAX_SHOTS = 5;

ShotManager::ShotManager(int owner) : owner(owner) {}

bool ShotManager::step(const TankState& owner_state, Round& round){
	vector<int> removed_shots;
	for(int shot: shots) if(round.get_shot(shot) == nullptr) removed_shots.push_back(shot);
	for(int shot: removed_shots) shots.erase(shot);
	
	if(owner_state.key_state.shoot && shots.size() < MAX_SHOTS){
		shots.insert(round.add_shot(make_unique<Shot>(ShotDetails(
			owner_state.position + owner_state.direction * CANNON_LENGTH,
			owner_state.direction * SHOT_SPEED,
			SHOT_RADIUS, SHOT_TTL
		), owner)));
	}
	return true;
}

void ShotManager::reset(){
	shots.clear();
}


Tank::Tank(Game& game, int index) :
	game(game),
	index(index),
	shot_manager(index),
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
	
	shot_manager.reset();
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
void Tank::advance(Round& round){
	if(state.active){
		state.key_state = pending_keys.front();
		pending_keys.pop_front();
	}
	else{
		state.key_state = KeyState();
	}

	advance_tank(state, game.get_maze());
	
	shot_manager.step(state, round);
}

bool Projectile::advance(Game& game){
	vector<int> killed_tanks;
	
	bool finished = step(game.get_maze(), game.get_states(), killed_tanks);
	
	for(auto tank: killed_tanks){
		// Kill tank
	}
	
	return finished;
}

Shot::Shot(ShotDetails&& details, int shooter) : state(move(details)), ignored_tank(shooter) {}

bool Shot::step(
	const Maze& maze, const vector<const TankState*>& tanks,
	vector<int>& killed_tanks
){
	path.clear();
	path.push_back(state.position);

	int tank_hit = advance_shot(state, maze, tanks, ignored_tank, path);
	if(tank_hit >= 0){
		killed_tanks.push_back(tank_hit);
		return true;
	}

	if(state.timer > 0) state.timer--;
	return state.timer == 0;
}

const ShotDetails& Shot::get_state() const{
	return state;
}
const vector<Point>& Shot::get_path() const{
	return path;
}
