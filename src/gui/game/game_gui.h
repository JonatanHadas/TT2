#ifndef _GAME_GUI_H
#define _GAME_GUI_H

#include "../gui.h"

#include "game_drawer.h"

class GameGui : public Gui{
	GameDrawer drawer;

	GameView* view;

public:
	GameGui(
		GameView* view
	);

	GameGui(GameGui&&) = delete;
	GameGui(const GameGui&) = delete;
	
	~GameGui();
	
	GameGui& operator=(GameGui&&) = delete;
	GameGui& operator=(const GameGui&) = delete;

	bool step();
	bool handle_event(const SDL_Event& event);
	void draw(SDL_Renderer* renderer);
};

#endif
