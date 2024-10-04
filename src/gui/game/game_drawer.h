#ifndef _GAME_DRAWER_H
#define _GAME_DRAWER_H

#include "../../game/data/game_settings.h"
#include "../../game/interface/game_view.h"

#include "../utils/utils.h"

#include "tank_images.h"

#include <memory>
#include <map>
#include <deque>
#include <SDL.h>

struct LaserData{
	int start = 0;
	deque<vector<TimePoint>> path;
	Point position = { .x = 0, .y = 0 };
	SDL_Color color;
};

class BoardDrawer{
	const GameSettings& settings;

	GameView* view;

	int maze_w, maze_h;
	unique_ptr<Texture> texture, laser_layer;

	vector<TankImage> tank_images;
	unique_ptr<Texture> circle_texture, shrapnel_texture;
	map<int, LaserData> lasers;
	
	void initialize(SDL_Renderer* renderer);
	void resize_canvas(SDL_Renderer* renderer);
	
	void draw_maze(SDL_Renderer* renderer);
	void draw_upgrades(SDL_Renderer* renderer);
	void draw_shots(SDL_Renderer* renderer);
	void draw_lasers(SDL_Renderer* renderer);
	void draw_shrapnel(SDL_Renderer* renderer);
	void draw_tanks(SDL_Renderer* renderer);
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
 