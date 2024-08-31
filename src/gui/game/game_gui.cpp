#include "game_gui.h"

GameGui::GameGui(
	GameView* view
) :
	drawer(view),
	view(view) {
		
}

GameGui::~GameGui(){
}

bool GameGui::step(){
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
	return false;
}

void GameGui::draw(SDL_Renderer* renderer){
	drawer.draw(renderer);
}
