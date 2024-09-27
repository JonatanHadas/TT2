#include "game_drawer.h"

#include "../utils/images.h"

#include "../utils/colors.h"

#include "../../game/logic/geometry.h"

#include "../../game/logic/logic.h"

#include <map>

#define DRAW_SCALE 100

const Number CANNON_WIDTH = Number(30) / 100;
const Number CANNON_LENGTH = Number(54) / 100;

#define _USE_MATH_DEFINES
#include <math.h>
#define RAD2DEG(x) ((x)*180 / M_PI)

static inline double angle(const Point& point){
	
	return RAD2DEG(atan2((float)(double)point.x, (float)(double)-point.y));
}

#define UPGRADES "upgrades/"

const map<Upgrade::Type, const unique_ptr<Texture>&> upgrade_textures({
	{ Upgrade::Type::GATLING, register_image(UPGRADES "gatling") },
	{ Upgrade::Type::LASER, register_image(UPGRADES "laser") },
});

#include <iostream>

BoardDrawer::BoardDrawer(GameView* view, const GameSettings& settings) :
	view(view),
	settings(settings),
	circle_texture(nullptr),
	texture(nullptr) {

}

const int LASER_DECAY_TIME = 10;

constexpr int CIRCLE_RADIUS = 50;
constexpr unsigned int CIRCLE_POINTS = 100;

const Texture& get_cannon_image(const TankUpgradeState* upgrade, const TankImage& image){
	if(upgrade == nullptr) return image.cannon;
	
	switch(upgrade->type){
	case Upgrade::Type::GATLING: 
	{
		int index = upgrade->timer % GATLING_IMAGE_CNT;
		if(index < 0) index += GATLING_IMAGE_CNT;
		return image.gatling[index];
	}
	case Upgrade::Type::LASER:
		return image.laser_gun;
	default:
		return image.cannon;
	}
}

void BoardDrawer::initialize(SDL_Renderer* renderer){
	if(circle_texture == nullptr){
		circle_texture = make_unique<Texture>(renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			CIRCLE_RADIUS * 2, CIRCLE_RADIUS * 2
		);
		
		circle_texture->do_with_texture(renderer, [&](){
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);
			
			vector<SDL_Vertex> vertices;
			vector<int> indices;
			vertices.push_back({
				.position = { .x = CIRCLE_RADIUS, .y = CIRCLE_RADIUS },
				.color = { .r = 255, .g = 255, .b = 255, .a = 255 },
				.tex_coord = { .x = 0, .y = 0 }
			});
			for(int i = 0; i < CIRCLE_POINTS; i++){
				double angle = 2 * M_PI * i / CIRCLE_POINTS;
				vertices.push_back({
					.position = { .x = CIRCLE_RADIUS * (float)(1 + cos(angle)), .y = CIRCLE_RADIUS * (float)(1 + sin(angle)) },
					.color = { .r = 255, .g = 255, .b = 255, .a = 255 },
					.tex_coord = { .x = 0, .y = 0 }
				});
				indices.push_back(0);
				indices.push_back(i + 1);
				indices.push_back((i + 1) % CIRCLE_POINTS + 1);
			}
			SDL_RenderGeometry(
				renderer, NULL,
				&vertices[0], vertices.size(),
				&indices[0], indices.size()
			);
		});
	}
	if(tank_images.empty()){
		for(int i = 0; i < view->get_states().size(); i++){
			tank_images.push_back(TANK_TEXTURES[i]->get_tank_image(renderer));
		}
	}
}

void BoardDrawer::resize_canvas(SDL_Renderer* renderer){
	if(
		texture == nullptr ||
		maze_w != view->get_maze().get_w() ||
		maze_h != view->get_maze().get_h()
	) {
		int texture_w = (view->get_maze().get_w() + 2*WALL_WIDTH) * DRAW_SCALE;
		int texture_h = (view->get_maze().get_h() + 2*WALL_WIDTH) * DRAW_SCALE;
		texture = make_unique<Texture>(renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			texture_w, texture_h
		);
		laser_layer = make_unique<Texture>(renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			texture_w, texture_h
		);
		SDL_SetTextureBlendMode(laser_layer->get(), SDL_BLENDMODE_BLEND);
		maze_w = view->get_maze().get_w();
		maze_h = view->get_maze().get_h();
	}
}

void BoardDrawer::draw_maze(SDL_Renderer* renderer){
	SDL_SetRenderDrawColor(renderer, board_color.r, board_color.g, board_color.b, board_color.a);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, wall_color.r, wall_color.g, wall_color.b, wall_color.a);
	SDL_Rect rect;
	rect.w = (2 * WALL_WIDTH + 1) * DRAW_SCALE;
	rect.h = 2 * WALL_WIDTH * DRAW_SCALE;
	for(int x = 0; x < view->get_maze().get_w(); x++){
		rect.x = x * DRAW_SCALE;
		for(int y = 0; y <= view->get_maze().get_h(); y++){
			rect.y = y * DRAW_SCALE;
			if(view->get_maze().has_hwall_below(x, y - 1)) SDL_RenderFillRect(renderer, &rect);
		}
	}
	rect.w = 2 * WALL_WIDTH * DRAW_SCALE;
	rect.h = (2 * WALL_WIDTH + 1) * DRAW_SCALE;
	for(int x = 0; x <= view->get_maze().get_w(); x++){
		rect.x = x * DRAW_SCALE;
		for(int y = 0; y < view->get_maze().get_h(); y++){
			rect.y = y * DRAW_SCALE;
			if(view->get_maze().has_vwall_right(x - 1, y)) SDL_RenderFillRect(renderer, &rect);
		}
	}
}

void BoardDrawer::draw_upgrades(SDL_Renderer* renderer){
	SDL_Rect upgrade_rect;
	upgrade_rect.w = upgrade_rect.h = UPGRADE_SIZE * DRAW_SCALE;
	for(const auto& upgrade: view->get_upgrades()){
		upgrade_rect.x = DRAW_SCALE * (upgrade->x + Number(1) / 2 + WALL_WIDTH ) - upgrade_rect.w / 2;
		upgrade_rect.y = DRAW_SCALE * (upgrade->y + Number(1) / 2 + WALL_WIDTH ) - upgrade_rect.h / 2;
		
		SDL_RenderCopyEx(
			renderer,
			upgrade_textures.at(upgrade->type)->get(),
			NULL, &upgrade_rect,
			angle(UPGRADE_ROTATION), NULL,
			SDL_FLIP_NONE
		);
	}
}

void BoardDrawer::draw_shots(SDL_Renderer* renderer){
	auto shots = view->get_shots();
	for(const auto& shot: shots){
		switch(shot.state.type){
		case ShotDetails::Type::ROUND:
			{
				SDL_Rect shot_rect;
				shot_rect.w = shot_rect.h = DRAW_SCALE * shot.state.radius * 2;
				
				shot_rect.x = DRAW_SCALE * (shot.state.position.x + WALL_WIDTH)- shot_rect.w / 2;
				shot_rect.y = DRAW_SCALE * (shot.state.position.y + WALL_WIDTH)- shot_rect.h / 2;
				
				SDL_SetTextureColorMod(circle_texture->get(), 0, 0, 0);
				SDL_SetTextureAlphaMod(circle_texture->get(), 255);
				
				SDL_RenderCopy(renderer, circle_texture->get(), NULL, &shot_rect);
			}
			break;
		case ShotDetails::Type::LASER:
			if(lasers.count(shot.id) == 0) lasers.insert({shot.id, {
				.start = 0,
				.path = deque<vector<TimePoint>>(),
				.position = shot.state.position,
				.color = tank_images[shot.state.owner].texture.random_color()
			}});
			lasers[shot.id].path.push_front(shot.path);
			lasers[shot.id].position = shot.state.position;
			lasers[shot.id].start = 0;
			break;
		}
	}
	
	draw_lasers(renderer);
}

void BoardDrawer::draw_lasers(SDL_Renderer* renderer){
	set<int> finished_lasers;
	for(auto& [id, laser]: lasers){
		laser_layer->do_with_texture(renderer, [&](){
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);
			
			SDL_Color color;
			color.r = laser.color.r;
			color.g = laser.color.g;
			color.b = laser.color.b;
			color.a = (int)(255 * (1 - (laser.start / LASER_DECAY_TIME)));

			vector<SDL_Vertex> vertices;
			vector<int> indices;
			Point last = laser.position;

			int i = 0;
			for(
				Number start = laser.start;
				i < laser.path.size() && start < LASER_DECAY_TIME;
				start += laser.path[i++][0].time
			){
				for(int j = laser.path[i].size() - 1; j >= 0; j--){
					indices.push_back(vertices.size());
					indices.push_back(vertices.size() + 1);
					indices.push_back(vertices.size() + 2);
					indices.push_back(vertices.size() + 1);
					indices.push_back(vertices.size() + 2);
					indices.push_back(vertices.size() + 3);

					auto direction = laser.path[i][j].point - last;
					normalize(direction);
					direction *= LASER_RADIUS;
					vertices.push_back({
						.position = {
							.x = (float)(double)(DRAW_SCALE * (WALL_WIDTH + last.x + direction.y)),
							.y = (float)(double)(DRAW_SCALE * (WALL_WIDTH + last.y - direction.x))
						},
						.color = color,
						.tex_coord = { .x = 0, .y = 0 }
					});
					vertices.push_back({
						.position = {
							.x = (float)(double)(DRAW_SCALE * (WALL_WIDTH + last.x - direction.y)),
							.y = (float)(double)(DRAW_SCALE * (WALL_WIDTH + last.y + direction.x))
						},
						.color = color,
						.tex_coord = { .x = 0, .y = 0 }
					});
					
					if(start + laser.path[i][j].time > LASER_DECAY_TIME){
						color.a = 0;
					}
					else{
						color.a = (int)(255 * (1 - (start + laser.path[i][j].time) / LASER_DECAY_TIME));
					}
					last = laser.path[i][j].point;
					
					vertices.push_back({
						.position = {
							.x = (float)(double)(DRAW_SCALE * (WALL_WIDTH + last.x + direction.y)),
							.y = (float)(double)(DRAW_SCALE * (WALL_WIDTH + last.y - direction.x))
						},
						.color = color,
						.tex_coord = { .x = 0, .y = 0 }
					});
					vertices.push_back({
						.position = {
							.x = (float)(double)(DRAW_SCALE * (WALL_WIDTH + last.x - direction.y)),
							.y = (float)(double)(DRAW_SCALE * (WALL_WIDTH + last.y + direction.x))
						},
						.color = color,
						.tex_coord = { .x = 0, .y = 0 }
					});
					
					if(start + laser.path[i][j].time > LASER_DECAY_TIME) break;
				}
			}
			if(i == 0) finished_lasers.insert(id);
			while(laser.path.size() > i) laser.path.pop_back();
			
			SDL_RenderGeometry(
				renderer, NULL,
				&vertices[0], vertices.size(),
				&indices[0], indices.size()
			);
			
			laser.start++;
		});
		
		SDL_RenderCopy(renderer, laser_layer->get(), NULL, NULL);
	}
	for(int id: finished_lasers){
		lasers.erase(id);
	}
}

void BoardDrawer::draw_tanks(SDL_Renderer* renderer){
	auto tank_states = view->get_states();
	for(int i = 0; i < tank_states.size(); i++){
		if(!tank_states[i].state.alive) continue;
		
		Number draw_x = (tank_states[i].state.position.x + WALL_WIDTH) * DRAW_SCALE;
		Number draw_y = (tank_states[i].state.position.y + WALL_WIDTH) * DRAW_SCALE;
		auto direction = angle(tank_states[i].state.direction);
		
		SDL_Rect tank_rect;
		tank_rect.w = DRAW_SCALE * TANK_WIDTH;
		tank_rect.h = DRAW_SCALE * TANK_LENGTH;
		tank_rect.x = draw_x - tank_rect.w / 2;
		tank_rect.y = draw_y - tank_rect.h / 2;
		
		SDL_RenderCopyEx(
			renderer,
			tank_images[i].body.get(),
			NULL, &tank_rect,
			direction, NULL,
			SDL_FLIP_NONE
		);
		
		tank_rect.w = DRAW_SCALE * CANNON_WIDTH;
		tank_rect.h = DRAW_SCALE * CANNON_LENGTH;
		tank_rect.x = draw_x - tank_rect.w / 2;
		tank_rect.y = draw_y - tank_rect.h / 2;
		SDL_RenderCopyEx(
			renderer,
			get_cannon_image(tank_states[i].upgrade, tank_images[i]).get(),
			NULL, &tank_rect,
			direction, NULL,
			SDL_FLIP_NONE
		);
	}
}


void BoardDrawer::draw(SDL_Renderer* renderer){
	resize_canvas(renderer);
	initialize(renderer);
		
	texture->do_with_texture(renderer, [&](){
		draw_maze(renderer);
		draw_upgrades(renderer);
		draw_shots(renderer);
		draw_tanks(renderer);
	});
}

Texture& BoardDrawer::get_texture() const{
	return *texture;
}

GameDrawer::GameDrawer(GameView* view, const GameSettings& settings) :
	view(view),
	settings(settings),
	board_drawer(view, settings),
	is_initialized(false) {
		
		
}

void GameDrawer::init(SDL_Renderer* renderer){
	if(!is_initialized){
		SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);

		SDL_RenderSetLogicalSize(renderer, screen_width, screen_height);

		is_initialized = true;		
	}
}

void GameDrawer::draw(SDL_Renderer* renderer){
	init(renderer);

	SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	SDL_RenderClear(renderer);
	
	board_drawer.draw(renderer);
	SDL_Rect board_rect;
	int w, h;
	SDL_QueryTexture(board_drawer.get_texture().get(), NULL, NULL, &w, &h);
	
	int max_width = screen_width * 0.9;
	int max_height = screen_height * 0.9;
	
	board_rect.w = max_width * h < max_height * w ? max_width : max_height * w / h;
	board_rect.h = max_width * h > max_height * w ? max_height : max_width * h / w;
	board_rect.x = (screen_width - board_rect.w) / 2;
	board_rect.y = (screen_height - board_rect.h) / 2;
	
    SDL_RenderCopy(renderer, board_drawer.get_texture().get(), NULL, &board_rect);
}

void GameDrawer::step(){

}