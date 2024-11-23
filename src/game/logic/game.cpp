#include "game.h"

#include "../../utils/utils.h"

#include "geometry.h"
#include "logic.h"

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
vector<MissileState> Game::get_missiles() const{
	vector<MissileState> missiles;
	for(const auto& entry: round->get_missiles()){
		missiles.push_back({
			.state=entry.second->get_state(),
			.target=entry.second->get_target()
		});
	}
	return missiles;
}
vector<const ShrapnelState*> Game::get_shrapnels() const{
	vector<const ShrapnelState*> shrapnels;
	for(const auto& shrapnel: round->get_shrapnels()){
		shrapnels.push_back(&shrapnel->get_state());
	}
	return shrapnels;
}
vector<MineCompleteState> Game::get_mines() const{
	vector<MineCompleteState> mines;
	for(const auto& entry: round->get_mines()){
		mines.push_back({
			.details=entry.second->get_details(),
			.state=entry.second->get_state()
		});
	}
	return mines;
}
vector<DeathRayState> Game::get_death_rays() const{
	vector<DeathRayState> death_rays;
	for(const auto& [id, death_ray]: round->get_death_rays()){
		death_rays.push_back({
			.path = death_ray->get_path(),
			.timer = death_ray->get_timer()
		});
	}
	return death_rays;
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
	maze(generate_maze(maze_generation, rand_range(5, 12), rand_range(5, 12))),
	maze_map(maze) {

}

const Maze& Round::get_maze() const{
	return maze;
}
const MazeMap& Round::get_maze_map() const{
	return maze_map;
}

int Round::add_shot(unique_ptr<Shot>&& shot){
	int shot_id = next_id++;
	shots.insert({shot_id, move(shot)});
	return shot_id;
}
void Round::remove_shot(int shot_id){
	game.on_shot_removed(shot_id);
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

int Round::add_missile(unique_ptr<Missile>&& missile){
	int missile_id = next_id++;
	missiles.insert({missile_id, move(missile)});
	return missile_id;
}
void Round::remove_missile(int missile_id){
	game.on_missile_removed(missile_id);
	missiles.erase(missile_id);
}
Missile* Round::get_missile(int missile_id) const{
	auto it = missiles.find(missile_id);
	if(it == missiles.end()) return nullptr;
	return it->second.get();
}
const map<int, unique_ptr<Missile>>& Round::get_missiles() const{
	return missiles;
}

int Round::add_mine(unique_ptr<Mine>&& mine){
	int mine_id = next_id++;
	mines.insert({mine_id, move(mine)});
	return mine_id;
}
void Round::remove_mine(int mine_id){
	explode(mines[mine_id]->get_details().position);
	mines.erase(mine_id);
}
Mine* Round::get_mine(int mine_id) const{
	auto it = mines.find(mine_id);
	if(it == mines.end()) return nullptr;
	return it->second.get();
}
const map<int, unique_ptr<Mine>>& Round::get_mines() const{
	return mines;
}

int Round::add_death_ray(unique_ptr<DeathRay>&& death_ray){
	int death_ray_id = next_id++;
	death_rays.insert({death_ray_id, move(death_ray)});
	return death_ray_id;
}
void Round::remove_death_ray(int death_ray_id){
	game.on_death_ray_removed(death_ray_id);
	death_rays.erase(death_ray_id);
}
DeathRay* Round::get_death_ray(int death_ray_id) const{
	auto it = death_rays.find(death_ray_id);
	if(it == death_rays.end()) return nullptr;
	return it->second.get();
}
const map<int, unique_ptr<DeathRay>>& Round::get_death_rays() const{
	return death_rays;
}



const Number EXPLOSION_SIZE = 5;
const Number MIN_EXPLOSION_RANGE = 4;
const int EXPLOSION_SHRAPNEL_COUNT = 100;

void Round::explode(const Point& source){
	vector<ShrapnelDetails> new_shrapnels;
	for(int i = 0; i < EXPLOSION_SHRAPNEL_COUNT; i++){
		new_shrapnels.push_back(ShrapnelDetails(
			source, random_direction() * Number::random(MIN_EXPLOSION_RANGE, EXPLOSION_SIZE)
		));
	}
	for(const auto& shrapnel: new_shrapnels) shrapnels.insert(make_unique<Shrapnel>(
		shrapnel, maze
	));
}
const set<unique_ptr<Shrapnel>>& Round::get_shrapnels() const{
	return shrapnels;
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
	for(int shot_id: removed_shots){
		remove_shot(shot_id);
	}
	removed_shots.clear();
	for(const auto& shot_entry: shots){
		if(shot_entry.second->advance(game)){
			removed_shots.insert(shot_entry.first);
		}
	}

	for(int missile_id: removed_missiles){
		remove_missile(missile_id);
	}
	removed_missiles.clear();
	for(const auto& missile_entry: missiles){
		if(missile_entry.second->advance(game)){
			removed_missiles.insert(missile_entry.first);
		}
	}
	
	vector<int> removed_mines;
	for(const auto& mine_entry: mines){
		if(mine_entry.second->step(game.get_states())){
			removed_mines.push_back(mine_entry.first);
		}
	}
	for(int mine_id: removed_mines){
		remove_mine(mine_id);
	}

	vector<int> removed_death_rays;
	for(const auto& [id, death_ray]: death_rays){
		if(death_ray->advance(game)){
			removed_death_rays.push_back(id);
		}
	}
	for(int death_ray_id: removed_death_rays){
		remove_death_ray(death_ray_id);
	}
	
	vector<const unique_ptr<Shrapnel>*> removed_shrapnel;
	for(const auto& shrapnel: shrapnels){
		if(shrapnel->advance(game)) removed_shrapnel.push_back(&shrapnel);
	}
	for(auto shrapnel: removed_shrapnel){
		shrapnels.erase(*shrapnel);
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

WeaponManager::WeaponManager() {}
WeaponManager::~WeaponManager() {}

const Number CANNON_LENGTH = Number(17)/100;

const Number SHOT_RADIUS = Number(3)/100;
const Number SHOT_SPEED = Number(1)/25;
const int SHOT_TTL = 1200;
const int MAX_SHOTS = 5;

ShotManager::ShotManager(int owner, Game& game) : owner(owner), game(game) {
	game.add_observer(this);
}
ShotManager::~ShotManager() {
	game.remove_observer(this);
}

void ShotManager::on_shot_removed(int shot_id){
	if(shots.count(shot_id)) shots.erase(shot_id);
}

bool ShotManager::step(
	const TankState& owner_state,
	const KeyState& previous_keys,
	Round& round
){
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

BombManager::BombManager(int owner, Game& game) :
	AppliedUpgrade({
		.type = Upgrade::Type::BOMB,
		.state = 0,
		.timer = 0,
	}),
	game(game),
	round(nullptr),
	owner(owner) {
	
	game.add_observer(this);
}

BombManager::~BombManager(){
	game.remove_observer(this);
}

void BombManager::on_shot_removed(int shot_id){
	if(round != nullptr && state.state && shot_id == shot) {
		round->explode(round->get_shot(shot)->get_state().position);
	}
}

const Number BOMB_SPEED = Number(4) / 100;
const Number BOMB_RADIUS = Number(5) / 100;

bool BombManager::step(
	const TankState& owner_state,
	const KeyState& previous_keys,
	Round& round
){
	this->round = &round;
	if(state.state && round.get_shot(shot) == nullptr) return true;

	if(owner_state.key_state.shoot && !previous_keys.shoot) {
		if(state.state){
			round.remove_shot(shot);
			return true;
		} else {
			state.state = 1;
			shot = round.add_shot(make_unique<Shot>(ShotDetails(
				owner_state.position + owner_state.direction * CANNON_LENGTH,
				owner_state.direction * BOMB_SPEED,
				BOMB_RADIUS, -1, ShotDetails::Type::ROUND,
				owner
			)));
		}
	}
	return false;
}

RemoteControlMissileManager::RemoteControlMissileManager(int owner) :
	AppliedUpgrade({
		.type = Upgrade::Type::RC_MISSILE,
		.state = 0,
		.timer = 0
	}),
	controller(nullptr),
	owner(owner) {
}

bool RemoteControlMissileManager::allow_moving() const {
	return state.state == 0;
}

bool RemoteControlMissileManager::step(
	const TankState& owner_state,
	const KeyState& previous_keys,
	Round& round
	){
	
	if(state.state){
		if(round.get_missile(missile) == nullptr) return true;
		controller->steer((owner_state.key_state.right ? 1 : 0) - (owner_state.key_state.left ? 1 : 0));
	}
	else{
		if(owner_state.key_state.shoot && !previous_keys.shoot){
			state.state = 1;
			auto missile_control = make_unique<RemoteMissileController>();
			controller = missile_control.get();
			missile = round.add_missile(make_unique<Missile>(
				MissileDetails(
					owner_state.position + owner_state.direction * MISSILE_LAUNCHER_LENGTH,
					owner_state.direction,
					owner
				),
				move(missile_control)
			));
		}
	}
	
	return false;
}

HomingMissileManager::HomingMissileManager(int owner) :
	AppliedUpgrade({
		.type = Upgrade::Type::HOMING_MISSILE,
		.state = 0,
		.timer = 0
	}),
	owner(owner) {
}

bool HomingMissileManager::step(
	const TankState& owner_state,
	const KeyState& previous_keys,
	Round& round
	){
	if(state.state){
		return round.get_missile(missile) == nullptr;
	}
	else{
		if(owner_state.key_state.shoot && !previous_keys.shoot){
			state.state = 1;
			missile = round.add_missile(make_unique<Missile>(
				MissileDetails(
					owner_state.position + owner_state.direction * MISSILE_LAUNCHER_LENGTH,
					owner_state.direction,
					owner
				),
				make_unique<HomingMissileController>(round.get_maze_map())
			));
		}
	}
	
	return false;
}

MineManager::MineManager(int owner) : 
	AppliedUpgrade({
		.type = Upgrade::Type::MINES,
		.state = 0,
		.timer = 0
	}),
	owner(owner),
	remaining_mines(3) {
	
}

bool MineManager::step(
	const TankState& owner_state,
	const KeyState& previous_keys,
	Round& round
){
	if(owner_state.key_state.shoot && !previous_keys.shoot){
		round.add_mine(make_unique<Mine>(MineDetails(
			owner_state.position - owner_state.direction * MINE_DISTANCE,
			owner_state.direction,
			owner
		)));
		remaining_mines--;
	}
	return remaining_mines == 0;
}

DeathRayManager::DeathRayManager(int owner, Game& game) :
	AppliedUpgrade({
		.type = Upgrade::Type::DEATH_RAY,
		.state = 0,
		.timer = 0,
	}),
	owner(owner),
	game(game) {}
	
const Number DEATH_RAY_STEP = Number(3)/10;
const Number DEATH_RAY_MAX_TURN = Number(1)/20;

vector<Point> DeathRayManager::get_path(const TankState& owner_state){
	vector<Point> path;
	
	path.push_back(owner_state.position + owner_state.direction * CANNON_LENGTH);
	Point direction = owner_state.direction;
	
	auto tanks = game.get_states();
	
	while(
		path.back().x > 0 && path.back().x < game.get_maze().get_w() &&
		path.back().y > 0 && path.back().y < game.get_maze().get_h()
	){
		path.push_back(path.back() + direction * DEATH_RAY_STEP);
		
		Number turn = 0, weight = 0;
		bool first = true;
		for(int i = 0; i < tanks.size(); i++){
			if(i == owner) continue;
			if(!tanks[i].state.alive) continue;
			Point way = tanks[i].state.position - path.back();
			
			Number distance_sqr = dot(way, way);
			Number distance_forward = dot(way, direction);
			if(distance_sqr == 0) continue;
			if(distance_forward < 0) continue;

			// TODO: find better weight function?
			Number current_weight = (distance_forward * distance_forward) / (distance_sqr * length(way));
			if(first || current_weight < weight){
				weight = current_weight;
				turn = cross(direction, way) / (2*distance_sqr);
			}
			first = false;
		}
		
		if(turn > DEATH_RAY_MAX_TURN) turn = DEATH_RAY_MAX_TURN;
		if(turn < -DEATH_RAY_MAX_TURN) turn = -DEATH_RAY_MAX_TURN;
		direction = rotate(direction, { .x = 1, .y = DEATH_RAY_STEP * turn });
		normalize(direction);
	}
	
	return path;
}

bool DeathRayManager::step(
	const TankState& owner_state,
	const KeyState& previous_keys,
	Round& round
){
	if(state.timer > 0) state.timer--;
	else{
		switch(state.state){
		case 0:
			if(owner_state.key_state.shoot && !previous_keys.shoot){
				state.state = 1;
				state.timer = DEATH_RAY_LOAD_TIME;
			}
			break;
		case 1:
			state.state = 2;
			death_ray = round.add_death_ray(make_unique<DeathRay>(DeathRayPath(
				get_path(owner_state),
				owner
			)));
			break;
		case 2:
			if(round.get_death_ray(death_ray) == nullptr) return true;
			break;
		}
	}
	return false;
}
bool DeathRayManager::allow_moving() const{
	return state.state == 0;
}

Tank::Tank(Game& game, int index) :
	game(game),
	index(index),
	shot_manager(make_unique<ShotManager>(index, game)),
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
	case Upgrade::Type::BOMB:
		upgrade = make_unique<BombManager>(index, game);
		break;
	case Upgrade::Type::RC_MISSILE:
		upgrade = make_unique<RemoteControlMissileManager>(index);
		break;
	case Upgrade::Type::HOMING_MISSILE:
		upgrade = make_unique<HomingMissileManager>(index);
		break;
	case Upgrade::Type::MINES:
		upgrade = make_unique<MineManager>(index);
		break;
	case Upgrade::Type::DEATH_RAY:
		upgrade = make_unique<DeathRayManager>(index, game);
		break;
	}
}

void Tank::reset(int maze_w, int maze_h){
	state.position.x = Number(2 * rand_range(0, maze_w) + 1) / 2;
	state.position.y = Number(2 * rand_range(0, maze_h) + 1) / 2;

	state.direction = random_discrete_direction();
	state.key_state = KeyState();
	pending_keys.clear();

	shot_manager->reset();
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
		shot_manager->step(state, previous_keys, round);
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

Shrapnel::Shrapnel(const ShrapnelDetails& details, const Maze& maze) :
	state({
		.details = details,
		.collision = get_shrapnel_wall_collision(details, maze),
		.timer = 0,
	}) {}

bool Shrapnel::step(
	const Maze& maze, const vector<const TankState*>& tanks,
	vector<int>& killed_tanks
) {
	auto start_fraction = get_shrapnel_way(state.timer++);
	if(state.timer > SHRAPNEL_TTL) return true;
	if(start_fraction > state.collision) return false;
	
	auto end_fraction = get_shrapnel_way(state.timer);
	if(end_fraction > state.collision) end_fraction = state.collision;
	for(int i = 0; i < tanks.size(); i++){
		auto fraction = get_shrapnel_tank_collision(state.details, *tanks[i]);
		if(start_fraction < fraction && fraction < end_fraction){
			killed_tanks.push_back(i);
		}
	}
	return false;
}

const ShrapnelState& Shrapnel::get_state() const{
	return state;
}

RemoteMissileController::RemoteMissileController() : turn_state(0) {}

int RemoteMissileController::get_turn_direction() const {
	return turn_state;
}
int RemoteMissileController::get_target() const {
	return -1;
}
void RemoteMissileController::step(const MissileDetails& missile, const vector<const TankState*>& tanks) {
	turn_state = 0;
}

void RemoteMissileController::steer(int direction){
	turn_state = direction;
}

const int HOMING_TIME = 60;

HomingMissileController::HomingMissileController(const MazeMap& maze_map) :
	maze_map(maze_map),
	timer(HOMING_TIME),
	target(-1), turn_state(0) {}

int HomingMissileController::get_turn_direction() const{
	return turn_state;
}
int HomingMissileController::get_target() const{
	return target;
}
void HomingMissileController::step(const MissileDetails& missile, const vector<const TankState*>& tanks){
	if(timer > 0){
		timer--;
		return;
	}
	
	turn_state = target_missile_turn(maze_map, missile, tanks, target);
}

const int MISSILE_TTL = 1200;

Missile::Missile(MissileDetails&& details, unique_ptr<MissileController>&& controller) :
	state(move(details)),
	controller(move(controller)),
	timer(MISSILE_TTL),
	ignoring_owner(true) {

}

bool Missile::step(
	const Maze& maze, const vector<const TankState*>& tanks,
	vector<int>& killed_tanks
){
	advance_missile(state, controller->get_turn_direction(), maze);
	
	for(int i = 0; i < tanks.size(); i++){
		if(check_missile_tank_collision(state, *tanks[i])){
			if(state.owner == i && ignoring_owner) continue;
			killed_tanks.push_back(i);
			return true;
		}
		else if(i == state.owner){
			ignoring_owner = false;
		}
	}
	
	controller->step(state, tanks);
	
	if(timer > 0) timer--;
	return timer == 0;
}

const MissileDetails& Missile::get_state() const{
	return state;
}

const int Missile::get_target() const{
	return controller->get_target();
}

const int MINE_TIME = 60;

Mine::Mine(MineDetails&& details) :
	details(move(details)),
	timer(MINE_START_TIME),
	started(false),
	pressed(false) {

}

bool Mine::step(const vector<TankCompleteState>& tanks){
	if(timer == 0){
		if(started) return true;
		started = true;
	}
	if(timer >= 0) {
		timer--;
		return false;
	}
		
	bool previously_pressed = pressed;
	pressed = false;
	for(const auto tank: tanks){
		if(check_mine_collision(details, tank.state)){
			pressed = true;
		}
	}
	if(!pressed && previously_pressed) timer = MINE_TIME;
	
	return false;
}

const MineDetails& Mine::get_details() const{
	return details;
}

MineState Mine::get_state() const{
	if(started && timer >= 0){
		return MineState::COUNTING;
	}
	if(pressed) return MineState::PRESSED;
	return MineState::INACTIVE;
}

const int DEATH_RAY_TTL = 30;

DeathRay::DeathRay(DeathRayPath&& path) : path(path), timer(DEATH_RAY_TTL) {}

bool DeathRay::step(
	const Maze& maze, const vector<const TankState*>& tanks,
	vector<int>& killed_tanks
){
	timer--;
	if(timer == 0) return true;

	for(int i = 0; i < tanks.size(); i++){
		if(i == path.owner) continue;
		if(check_death_ray_collision(path.path, *tanks[i])){
			killed_tanks.push_back(i);
		}
	}
	return false;
}

const DeathRayPath& DeathRay::get_path() const{
	return path;
}
int DeathRay::get_timer() const{
	return timer;
}
