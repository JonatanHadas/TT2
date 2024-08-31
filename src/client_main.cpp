#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>

#include "gui/gui.h"
#include "gui/game/game_gui.h"
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
	srand(time(NULL));

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

	Game game(MazeGeneration::EXPAND_TREE);

	GameGui gui(
		&game
	);

	mainloop(gui, renderer);
	
	return 0;
}