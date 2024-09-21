#ifndef _IMAGES_H
#define _IMAGES_H

#include "utils.h"

#include <string>
#include <memory>

using namespace std;

typedef SDL_Color(*filter_t)(const SDL_Color&);

const unique_ptr<Texture>& register_image(const string& name, filter_t filter = nullptr);

template<size_t N>
const array<const unique_ptr<Texture>*, N> register_image(const string& name, array<filter_t, N> filters){
	array<const unique_ptr<Texture>*, N> textures;
	for(int i = 0; i < N; i++){
		textures[i] = &register_image(name, filters[i]);
	}
	return textures;
}

bool load_images(SDL_Renderer* renderer);

#endif
