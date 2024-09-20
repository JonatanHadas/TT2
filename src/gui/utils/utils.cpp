#include "utils.h"

Texture::Texture(
	SDL_Renderer* renderer,
	unsigned int format, int access,
	int width, int height
	) : 
	texture(SDL_CreateTexture(renderer, format, access, width, height)){}

Texture::Texture(
	SDL_Renderer* renderer,
	SDL_Surface* surface
	) : 
	texture(SDL_CreateTextureFromSurface(renderer, surface)){}

Texture::Texture(Texture&& inst) :
	texture(inst.texture) {
		
	inst.texture = NULL;
}

Texture& Texture::operator=(Texture&& inst){
	texture = inst.texture;
	inst.texture = NULL;
	
	return *this;
}

Texture::~Texture(){
	if(NULL != texture) SDL_DestroyTexture(texture);
}

SDL_Texture* Texture::get() const{
	return texture;
}

SDL_Texture* Texture::move_out(){
	SDL_Texture* result = texture;
	texture = NULL;
	
	return result;
}

void Texture::do_with_texture(SDL_Renderer* renderer, function<void()> todo){
	SDL_Texture* old_target = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, texture);
	
	todo();
	
	SDL_SetRenderTarget(renderer, old_target);
}
