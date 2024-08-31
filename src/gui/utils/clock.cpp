#include "clock.h"

#include <SDL.h>

Clock::Clock() :
	last_tick(SDL_GetTicks()),
	remainder(0) {}

void Clock::tick(double length){
	int time = SDL_GetTicks();
	int diff = time - last_tick;
	
	length -= remainder;
	while(diff <= length){
		SDL_Delay(length - diff);

		time = SDL_GetTicks();
		diff = time - last_tick;
	}
	remainder = diff - length;
	last_tick = time;
}
