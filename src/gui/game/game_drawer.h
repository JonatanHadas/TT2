#ifndef _GAME_DRAWER_H
#define _GAME_DRAWER_H

#include "../../game/interface/game_view.h"

#include "../utils/utils.h"

#include <memory>
#include <SDL.h>

class BoardDrawer{
	GameView* view;

	int maze_w, maze_h;
	unique_ptr<Texture> texture;
public:
	BoardDrawer(GameView* view);
	
	void draw(SDL_Renderer* renderer);
	
	Texture& get_texture() const;
};

class GameDrawer{
	GameView* view;

	BoardDrawer board_drawer;
	bool is_initialized;
	int screen_width, screen_height;

	void init(SDL_Renderer* renderer);
public:
	GameDrawer(GameView* view);
	
	void draw(SDL_Renderer* renderer);
	void step();
};

#endif
 