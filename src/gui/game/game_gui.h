#ifndef _GAME_GUI_H
#define _GAME_GUI_H

#include "../gui.h"

#include "../../game/data/game_settings.h"
#include "../../game/interface/game_view.h"
#include "../../game/interface/game_advancer.h"
#include "../../game/interface/player_interface.h"

#include "../controls/controller.h"

#include "game_drawer.h"

#include <memory>
#include <map>

using namespace std;

class GameGui : public Gui{
	const GameSettings settings;
	
	GameDrawer drawer;

	GameView* view;
	GameAdvancer* advancer;
	map<PlayerInterface*, unique_ptr<Controller>> controllers;

public:
	GameGui(
		GameView* view,
		GameAdvancer* advancer,
		const GameSettings& settings,
		map<PlayerInterface*, unique_ptr<Controller>>&& controllers
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
