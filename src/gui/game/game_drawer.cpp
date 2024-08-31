#include "game_drawer.h"

#include "../utils/colors.h"

#define WALL_WIDTH 0.1

#define DRAW_SCALE 100

BoardDrawer::BoardDrawer(GameView* view) :
	view(view),
	texture(nullptr) {
	
}

void BoardDrawer::draw(SDL_Renderer* renderer){
	if(
		texture == nullptr ||
		maze_w != view->get_maze().get_w() ||
		maze_h != view->get_maze().get_h()
	) {
		texture = make_unique<Texture>(renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			(view->get_maze().get_w() + WALL_WIDTH) * DRAW_SCALE,
			(view->get_maze().get_h() + WALL_WIDTH) * DRAW_SCALE
		);
	}
	
	texture->do_with_texture(renderer, [&](){
		SDL_SetRenderDrawColor(renderer, board_color.r, board_color.g, board_color.b, board_color.a);
		SDL_RenderClear(renderer);
	
		SDL_SetRenderDrawColor(renderer, wall_color.r, wall_color.g, wall_color.b, wall_color.a);
		SDL_Rect rect;
		rect.w = (WALL_WIDTH + 1) * DRAW_SCALE;
		rect.h = WALL_WIDTH * DRAW_SCALE;
		for(int x = 0; x < view->get_maze().get_w(); x++){
			rect.x = x * DRAW_SCALE;
			for(int y = 0; y <= view->get_maze().get_h(); y++){
				rect.y = y * DRAW_SCALE;
				if(view->get_maze().has_hwall_below(x, y - 1)) SDL_RenderFillRect(renderer, &rect);
			}
		}
		rect.w = WALL_WIDTH * DRAW_SCALE;
		rect.h = (WALL_WIDTH + 1) * DRAW_SCALE;
		for(int x = 0; x <= view->get_maze().get_w(); x++){
			rect.x = x * DRAW_SCALE;
			for(int y = 0; y < view->get_maze().get_h(); y++){
				rect.y = y * DRAW_SCALE;
				if(view->get_maze().has_vwall_right(x - 1, y)) SDL_RenderFillRect(renderer, &rect);
			}
		}
	});
}

Texture& BoardDrawer::get_texture() const{
	return *texture;
}

GameDrawer::GameDrawer(GameView* view) :
	view(view),
	board_drawer(view),
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