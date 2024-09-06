#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "../../game/data/game_objects.h"

#include <SDL.h>

class Controller{
public:
	virtual void handle_event(const SDL_Event& event) = 0;
	virtual KeyState get_state() = 0;
};

#endif
