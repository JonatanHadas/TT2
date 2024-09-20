#include "images.h"

#include <SDL_image.h>

#include <map>

map<string, unique_ptr<Texture>> images;

const unique_ptr<Texture>& register_image(const string& name){
	if(images.count(name) == 0) images.insert({name, nullptr});
	return images[name];
}

void free_images(){
	for(auto& [name, texture]: images) texture = nullptr;
}

unique_ptr<Texture> load_image(SDL_Renderer* renderer, const string& name){
	string path = "data/images/" + name + ".png";
	SDL_Surface* surface = IMG_Load(path.c_str());
	
	if(surface == nullptr) return nullptr;
	
	unique_ptr<Texture> texture = make_unique<Texture>(renderer, surface);
	SDL_FreeSurface(surface);
	
	if(texture->get() == nullptr) return nullptr;
	return texture;
}

bool load_images(SDL_Renderer* renderer){
	atexit(free_images);
	
	for(auto& [name, texture]: images){
		texture = load_image(renderer, name);
		if(texture == nullptr) return false;
	}
	
	return true;
}
