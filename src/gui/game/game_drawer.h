#ifndef _GAME_DRAWER_H
#define _GAME_DRAWER_H

#include "../../game/data/game_settings.h"
#include "../../game/interface/game_view.h"

#include "../utils/utils.h"

#include <memory>
#include <SDL.h>

class BoardDrawer{
	const GameSettings& settings;

	GameView* view;

	int maze_w, maze_h;
	unique_ptr<Texture> texture;

	unique_ptr<Texture> tank_texture, upgrade_texture;  // Temp
	unique_ptr<Texture> circle_texture;
public:
	BoardDrawer(GameView* view, const GameSettings& settings);
	
	void draw(SDL_Renderer* renderer);
	
	Texture& get_texture() const;
};

class GameDrawer{
	const GameSettings& settings;

	GameView* view;

	BoardDrawer board_drawer;
	bool is_initialized;
	int screen_width, screen_height;

	void init(SDL_Renderer* renderer);
public:
	GameDrawer(GameView* view, const GameSettings& settings);
	
	void draw(SDL_Renderer* renderer);
	void step();
};

#endif
 