#include "game_drawer.h"

#include "../utils/colors.h"

#include "../../game/logic/geometry.h"

#include "../../game/logic/logic.h"

#define DRAW_SCALE 100

#define _USE_MATH_DEFINES
#include <math.h>
#define RAD2DEG(x) ((x)*180 / M_PI)

static inline double angle(const Point& point){
	
	return RAD2DEG(atan2((double)point.x, (double)-point.y));
}

BoardDrawer::BoardDrawer(GameView* view, const GameSettings& settings) :
	view(view),
	settings(settings),
	texture(nullptr) {

}

constexpr int CIRCLE_RADIUS = 50;
constexpr unsigned int CIRCLE_POINTS = 100;

void BoardDrawer::draw(SDL_Renderer* renderer){
	if(
		texture == nullptr ||
		maze_w != view->get_maze().get_w() ||
		maze_h != view->get_maze().get_h()
	) {
		texture = make_unique<Texture>(renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			(view->get_maze().get_w() + 2*WALL_WIDTH) * DRAW_SCALE,
			(view->get_maze().get_h() + 2*WALL_WIDTH) * DRAW_SCALE
		);
	}
	
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
	if(tank_texture == nullptr){
		tank_texture = make_unique<Texture>(renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			DRAW_SCALE * TANK_WIDTH, DRAW_SCALE * TANK_LENGTH
		);
		tank_texture->do_with_texture(renderer, [&](){
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderClear(renderer);
			
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_Rect rect;
			rect.x = rect.y = 0;
			rect.w = DRAW_SCALE * TANK_WIDTH;
			rect.h = DRAW_SCALE * TANK_LENGTH;
			SDL_RenderDrawRect(renderer, &rect);
			
			rect.w = DRAW_SCALE * TANK_WIDTH / 10;
			rect.h = DRAW_SCALE * TANK_LENGTH / 2;
			rect.x = (DRAW_SCALE * TANK_WIDTH - rect.w) / 2;
			rect.y = 0;
			SDL_RenderFillRect(renderer, &rect);
		});
	}
	
	texture->do_with_texture(renderer, [&](){
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
		
		auto shots = view->get_shots();
		for(const auto& shot: shots){
			SDL_Rect shot_rect;
			shot_rect.w = shot_rect.h = DRAW_SCALE * shot.state.radius * 2;
			
			shot_rect.x = DRAW_SCALE * (shot.state.position.x + WALL_WIDTH)- shot_rect.w / 2;
			shot_rect.y = DRAW_SCALE * (shot.state.position.y + WALL_WIDTH)- shot_rect.h / 2;
			
			SDL_SetTextureColorMod(circle_texture->get(), 0, 0, 0);
			SDL_SetTextureAlphaMod(circle_texture->get(), 255);
			
			SDL_RenderCopy(renderer, circle_texture->get(), NULL, &shot_rect);
		}
		
		auto tank_states = view->get_states();
		for(int i = 0; i < tank_states.size(); i++){
			SDL_Rect tank_rect;
			tank_rect.w = DRAW_SCALE * TANK_WIDTH;
			tank_rect.h = DRAW_SCALE * TANK_LENGTH;
			tank_rect.x = (tank_states[i]->position.x + WALL_WIDTH) * DRAW_SCALE - tank_rect.w / 2;
			tank_rect.y = (tank_states[i]->position.y + WALL_WIDTH) * DRAW_SCALE - tank_rect.h / 2;
			
			SDL_SetTextureColorMod(
				tank_texture->get(),
				tank_colors[settings.colors[i]].r,
				tank_colors[settings.colors[i]].g,
				tank_colors[settings.colors[i]].b
			);
			SDL_RenderCopyEx(
				renderer,
				tank_texture->get(),
				NULL, &tank_rect,
				angle(tank_states[i]->direction), NULL,
				SDL_FLIP_NONE
			);
		}
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