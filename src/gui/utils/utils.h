#ifndef _GUI_UTILS_H
#define _GUI_UTILS_H

#include <SDL.h>
#include <functional>

using namespace std;

class Texture{
	SDL_Texture* texture;
public:
	Texture(
		SDL_Renderer* renderer,
		unsigned int format, int access,
		int width, int height
	);
	
	Texture(const Texture& inst) = delete;
	Texture(Texture&& inst);
	
	Texture& operator=(const Texture& inst) = delete;
	Texture& operator=(Texture&& inst);
	
	~Texture();
	
	SDL_Texture* get();
	SDL_Texture* move_out();
	
	void do_with_texture(SDL_Renderer* renderer, function<void()> todo);
};

#endif
