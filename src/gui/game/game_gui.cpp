#include "game_gui.h"

GameGui::GameGui(
	GameView* view,
	GameAdvancer* advancer,
	const GameSettings& settings,
	map<PlayerInterface*, unique_ptr<Controller>>&& controllers
) :
	settings(settings),
	drawer(view, this->settings),
	view(view),
	advancer(advancer),
	controllers(move(controllers)) {

}

GameGui::~GameGui(){
}

bool GameGui::step(){
	for(auto& entry: controllers){
		entry.first->step(view->get_round(), entry.second->get_state());
	}
	
	advancer->allow_step();
	advancer->advance();

	drawer.step();
	return false;
}

bool GameGui::handle_event(const SDL_Event& event){
	switch(event.type){
	case SDL_KEYDOWN:
		switch(event.key.keysym.scancode){
		case SDL_SCANCODE_ESCAPE:
			return true;
		}
		break;
	}
	for(auto& entry: controllers){
		entry.second->handle_event(event);
	}
	return false;
}

void GameGui::draw(SDL_Renderer* renderer){
	drawer.draw(renderer);
}
