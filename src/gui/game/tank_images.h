#ifndef _TANK_IMAGES_H
#define _TANK_IMAGES_H

#include "../utils/utils.h"

#include <vector>
#include <memory>

using namespace std;

#define GATLING_IMAGE_CNT (3)

struct TankImage{
	Texture body;
	Texture cannon;
	Texture gatling[GATLING_IMAGE_CNT];
};

class TankTexture{
public:
	virtual TankImage get_tank_image(SDL_Renderer* renderer) const = 0;
};

extern const vector<unique_ptr<TankTexture>> TANK_TEXTURES;

#endif
