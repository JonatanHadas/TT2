#ifndef _IMAGES_H
#define _IMAGES_H

#include "utils.h"

#include <string>
#include <memory>

using namespace std;

const unique_ptr<Texture>& register_image(const string& name);

bool load_images(SDL_Renderer* renderer);

#endif
