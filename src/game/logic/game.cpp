#include "game.h"

#include "../../utils/utils.h"

#include "geometry.h"
#include "logic.h"
#include "maze.h"

Game::Game(
	MazeGeneration maze_generation,
	const set<Upgrade::Type> allowed_upgrades,
	int tank_num
) :
	maze_generation(maze_generation),
	allowed_upgrades(allowed_upgrades.begin(), allowed_upgrades.end()),
	tanks(),
	round_num(-1) {
		
	for(int i = 0; i < tank_num; i++){
		tanks.push_back(Tank(*this, i));
	}
		
	new_round();
}

void Game::new_round(){
	round_num += 1;

	round = make_unique<Round>(*this, maze_generation, allowed_upgrades);

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
vector<TankCompleteState> Game::get_states() const{
	vector<TankCompleteState> states;
	for(const auto& tank: tanks){
		const auto& upgrade = tank.get_upgrade();
		states.push_back({
			.state = tank.get_state(),
			.upgrade = upgrade == nullptr ? nullptr : &upgrade->get_state(),
		});
	}
	return states;
}
vector<ShotPath> Game::get_shots() const{
	vector<ShotPath> shots;
	for(const auto& entry: round->get_shots()){
		shots.push_back(ShotPath({
			.id=entry.first,
			.state=entry.second->get_state(),
			.path=entry.second->get_path()
		}))	;
	}

	return shots;
}
const set<unique_ptr<Upgrade>>& Game::get_upgrades() const{
	return round->get_upgrades();
}

void Game::advance(){
	while(can_step()) step();
}

void Game::allow_step(){}

void Game::kill_tank(int index){
	tanks[index].kill();
}
void Game::upgrade_tank(int index, Upgrade::Type type){
	tanks[index].set_upgrade(type);
}


const int MAX_UPGRADE_TIME = 120;
const int MIN_UPGRADE_TIME = 60;

Round::Round(Game& game, MazeGeneration maze_generation, const vector<Upgrade::Type>& allowed_upgrades) :
	game(game),
	allowed_upgrades(allowed_upgrades),
	next_id(0),
	upgrade_timer(rand_range(MIN_UPGRADE_TIME, MAX_UPGRADE_TIME)),
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

const set<unique_ptr<Upgrade>>& Round::get_upgrades() const{
	return upgrades;
}

void Round::create_upgrade(){
	if(allowed_upgrades.empty()) return;

	int x = rand_range(0, maze.get_w());
	int y = rand_range(0, maze.get_h());
	
	for(const auto& upgrade: upgrades){
		if(upgrade->x == x && upgrade->y == y) return;
	}
	for(const auto& tank: game.get_states()){
		int tank_x = tank.state.position.x;
		int tank_y = tank.state.position.y;
		if(tank_x == x && tank_y == y) return;
	}
	
	upgrades.insert(make_unique<Upgrade>(
		x, y,
		allowed_upgrades[rand_range(0, allowed_upgrades.size())]
	));
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
	
	upgrade_timer--;
	if(upgrade_timer == 0){
		create_upgrade();
		upgrade_timer = rand_range(MIN_UPGRADE_TIME, MAX_UPGRADE_TIME);
	}

	const auto tanks = game.get_states();
	for(int i = 0; i < tanks.size(); i++){
		if(tanks[i].upgrade != nullptr) continue;
		for(const auto& upgrade: upgrades){
			if(check_upgrade_collision(tanks[i].state, *upgrade)){
				game.upgrade_tank(i, upgrade->type);
				upgrades.erase(upgrade);
				break;
			}
		}
	}
}

const Number CANNON_LENGTH = Number(17)/100;

const Number SHOT_RADIUS = Number(3)/100;
const Number SHOT_SPEED = Number(1)/25;
const int SHOT_TTL = 1200;
const int MAX_SHOTS = 5;

ShotManager::ShotManager(int owner) : owner(owner) {}

bool ShotManager::step(
	const TankState& owner_state,
	const KeyState& previous_keys,
	Round& round
){
	vector<int> removed_shots;
	for(int shot: shots) if(round.get_shot(shot) == nullptr) removed_shots.push_back(shot);
	for(int shot: removed_shots) shots.erase(shot);

	if(owner_state.key_state.shoot && !previous_keys.shoot && shots.size() < MAX_SHOTS){
		shots.insert(round.add_shot(make_unique<Shot>(ShotDetails(
			owner_state.position + owner_state.direction * CANNON_LENGTH,
			owner_state.direction * SHOT_SPEED,
			SHOT_RADIUS, SHOT_TTL, ShotDetails::Type::ROUND,
			owner
		))));
	}
	return false;
}

void ShotManager::reset(){
	shots.clear();
}

AppliedUpgrade::AppliedUpgrade(TankUpgradeState state) : state(state) {}
const TankUpgradeState& AppliedUpgrade::get_state() const{
	return state;
}
bool AppliedUpgrade::allow_moving() const{
	return true;
}

void AppliedUpgrade::reset() {}


const Number GATLING_RADIUS = Number(3)/200;
const Number GATLING_SPEED = Number(1)/20;
const Number GATLING_VARIANCE = Number(1)/12;
const int GATLING_TTL = 600;
const int GATLING_INTERVAL = 10;
const int GATLING_START_TIME = 30;

GatlingShotManager::GatlingShotManager(int owner) : 
	AppliedUpgrade({
		.type = Upgrade::Type::GATLING,
		.state = 0,
		.timer = -GATLING_START_TIME
	}),
	owner(owner) {

}

bool GatlingShotManager::step(
	const TankState& owner_state,
	const KeyState& previous_keys,
	Round& round
){
	if(owner_state.key_state.shoot && !previous_keys.shoot) state.state = 1;
	if(state.state){
		if(!owner_state.key_state.shoot) return true;
		state.timer++;
		if(state.timer >= 0 && state.timer % GATLING_INTERVAL == 0){
			Point variance = { .x = 1, .y = rand_range(-1000, 1000) * GATLING_VARIANCE / 1000 };
			normalize(variance);
			
			round.add_shot(make_unique<Shot>(ShotDetails(
				owner_state.position + owner_state.direction * CANNON_LENGTH,
				rotate(owner_state.direction, variance) * GATLING_SPEED,
				GATLING_RADIUS, GATLING_TTL, ShotDetails::Type::ROUND,
				owner
			)));
		}
		
	}
	return false;
}


LaserManager::LaserManager(int owner) : 
	AppliedUpgrade({
		.type = Upgrade::Type::LASER,
		.state = 0,
		.timer = 0,
	}),
	owner(owner) {
	
}

bool LaserManager::step(
	const TankState& owner_state,
	const KeyState& previous_keys,
	Round& round
) {
	if(owner_state.key_state.shoot && !previous_keys.shoot) {
		round.add_shot(make_unique<Shot>(ShotDetails(
			owner_state.position + owner_state.direction * CANNON_LENGTH,
			owner_state.direction * LASER_SPEED,
			LASER_RADIUS, LASER_TTL, ShotDetails::Type::LASER,
			owner
		)));
		return true;
	}
	return false;
}


Tank::Tank(Game& game, int index) :
	game(game),
	index(index),
	shot_manager(index),
	upgrade(nullptr),
	state(
		{ .x = -1, .y = -1 } /*position*/,
		{ .x = 1, .y = 0 } /*direction*/,
		KeyState(),
		true /*active*/,
		true /*alive*/
	) {

}

const TankState& Tank::get_state() const{
	return state;
}
const unique_ptr<AppliedUpgrade>& Tank::get_upgrade() const{
	return upgrade;
}
void Tank::set_upgrade(Upgrade::Type type){
	switch(type){
	case Upgrade::Type::GATLING:
		upgrade = make_unique<GatlingShotManager>(index);
		break;
	case Upgrade::Type::LASER:
		upgrade = make_unique<LaserManager>(index);
		break;
	}
}

void Tank::reset(int maze_w, int maze_h){
	state.position.x = Number(2 * rand_range(0, maze_w) + 1) / 2;
	state.position.y = Number(2 * rand_range(0, maze_h) + 1) / 2;

	state.direction = random_direction();
	state.key_state = KeyState();
	pending_keys.clear();

	shot_manager.reset();
	upgrade = nullptr;

	state.alive = true;
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
	auto previous_keys = state.key_state;
	if(state.active){
		state.key_state = pending_keys.front();
		pending_keys.pop_front();
	}
	else{
		state.key_state = KeyState();
	}

	if(!state.alive) return;

	if(upgrade != nullptr){
		if(upgrade->allow_moving()){
			advance_tank(state, game.get_maze());
		}
		if(upgrade->step(state, previous_keys, round)) upgrade = nullptr;
	}
	else{
		advance_tank(state, game.get_maze());
		shot_manager.step(state, previous_keys, round);
	}
}

void Tank::kill(){
	state.alive = false;
}

bool Projectile::advance(Game& game){
	vector<int> killed_tanks;
	
	vector<const TankState*> tanks;
	for(const auto& tank: game.get_states()) tanks.push_back(&tank.state);

	bool finished = step(game.get_maze(), tanks, killed_tanks);

	for(int tank: killed_tanks){
		game.kill_tank(tank);
	}

	return finished;
}

Shot::Shot(ShotDetails&& details) : state(move(details)), ignored_tank(state.owner) {}

bool Shot::step(
	const Maze& maze, const vector<const TankState*>& tanks,
	vector<int>& killed_tanks
){
	path.clear();

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
const vector<TimePoint>& Shot::get_path() const{
	return path;
}
