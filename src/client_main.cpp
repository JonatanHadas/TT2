#include <iostream>
#include <SDL.h>

#include "gui/gui.h"
#include "gui/game/game_gui.h"
#include "gui/controls/keyset.h"
#include "game/logic/game.h"

using namespace std;

SDL_Window*  screen;
SDL_Renderer* renderer;

void close_rend(){
	SDL_DestroyRenderer(renderer);
}
void close_window(){
	SDL_DestroyWindow(screen);
}

int main(int argc, char** argv){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		cerr << "Error initializing SDL:" << endl << SDL_GetError() << endl;
		return 1;
	}
	atexit(SDL_Quit);

	screen = SDL_CreateWindow(
		"Tank Trouble",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		//960, 640, 0);
		0, 0,
		SDL_WINDOW_FULLSCREEN_DESKTOP
	);
		
	if(screen == NULL){
		cerr << "Error while opening window:" << endl << SDL_GetError() << endl;
		return 2;
	}
	atexit(close_window);
	
	renderer = SDL_CreateRenderer(screen, -1,SDL_RENDERER_TARGETTEXTURE);
	if(renderer == NULL){
		cerr << "Error while opening renderer:" << endl << SDL_GetError() << endl;
		return 3;
	}
	atexit(close_rend);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

	int screen_w, screen_h;
	SDL_GetRendererOutputSize(renderer, &screen_w, &screen_h);
	SDL_RenderSetLogicalSize(renderer, screen_w, screen_h);

	vector<int> colors;
	for(int i = 0; i < 3; i++) colors.push_back(i);
	GameSettings settings(
		colors
	);
	
	set<Upgrade::Type> allowed_upgrades({
		Upgrade::Type::DEATH_RAY
	});

	Game game(MazeGeneration::EXPAND_TREE, allowed_upgrades, settings.colors.size());
	
	map<PlayerInterface*, unique_ptr<Controller>> controllers;
	controllers.insert(make_pair(
		&game.get_player_interface(0),
		make_unique<KeyController>(KeySet({
			.left=SDL_SCANCODE_LEFT,
			.right=SDL_SCANCODE_RIGHT,
			.forward=SDL_SCANCODE_UP,
			.back=SDL_SCANCODE_DOWN,
			.shoot=SDL_SCANCODE_SPACE,
		}))
	));
	controllers.insert(make_pair(
		&game.get_player_interface(1),
		make_unique<KeyController>(KeySet({
			.left=SDL_SCANCODE_A,
			.right=SDL_SCANCODE_D,
			.forward=SDL_SCANCODE_W,
			.back=SDL_SCANCODE_S,
			.shoot=SDL_SCANCODE_TAB,
		}))
	));
	game.get_player_interface(2).set_active(false);
	
	GameGui gui(
		&game,
		&game,
		settings,
		move(controllers)
	);

	mainloop(gui, renderer);
	
	return 0;
}