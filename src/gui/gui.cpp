#include "gui.h"

#include "utils/clock.h"

#define TICK_LEN (1000.0 / 60.0)

void mainloop(Gui& gui, SDL_Renderer* renderer){
	Clock clock;
	
	while(true){
		gui.draw(renderer);
		
		SDL_RenderPresent(renderer);
		
		SDL_Event event;
		
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT) return;
			if(gui.handle_event(event)) return;
		}
		
		gui.step();
		
		clock.tick(TICK_LEN);
	}
}