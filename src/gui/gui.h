#ifndef _GUI_H
#define _GUI_H

#include <SDL.h>

class Gui{
public:
	virtual bool step() = 0;
	virtual bool handle_event(const SDL_Event& event) = 0;
	virtual void draw(SDL_Renderer* renderer) = 0;
};

void mainloop(Gui& gui, SDL_Renderer* renderer);

#endif
