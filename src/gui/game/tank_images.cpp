#include "tank_images.h"

#include "../utils/images.h"

#include "../../utils/utils.h"

struct TankSourceImage{
	const unique_ptr<Texture>& color;
	const unique_ptr<Texture>& light;
};

const array<filter_t, 2> filters = {
	[](const SDL_Color& color) -> SDL_Color {
		return { .r = 255, .g = 255, .b = 255, .a = color.r };
	},
	[](const SDL_Color& color) -> SDL_Color {
		return { .r = color.g, .g = color.g, .b = color.g, .a = color.a };
	}
};

TankSourceImage register_tank_image(const string name){
	auto [color, light] = register_image(name, filters);
	return {
		.color = *color,
		.light = *light
	};
}

#define TANK "tank/"

const TankSourceImage BODY = register_tank_image(TANK "body");
const TankSourceImage CANNON = register_tank_image(TANK "cannon");
const TankSourceImage GATLING[] = {
	register_tank_image(TANK "gatling0"),
	register_tank_image(TANK "gatling1"),
	register_tank_image(TANK "gatling2"),
};
const TankSourceImage LASER_GUN = register_tank_image(TANK "lasergun");
const TankSourceImage THICK_CANNON = register_tank_image(TANK "thick_cannon");
const TankSourceImage LAUNCHER = register_tank_image(TANK "launcher");
const TankSourceImage MISSILE = register_tank_image(TANK "missile");
const TankSourceImage MINE_ON = register_tank_image(TANK "mine_on");
const TankSourceImage MINE_OFF = register_tank_image(TANK "mine_off");
const TankSourceImage RAY_GUN[] = {
	register_tank_image(TANK "raygun0"),
	register_tank_image(TANK "raygun1"),
	register_tank_image(TANK "raygun2"),
	register_tank_image(TANK "raygun3"),
};
const TankSourceImage DEATH_RAY = register_tank_image(TANK "deathray");

class BaseTankTexture : public TankTexture{
	Texture get_image(SDL_Renderer* renderer, const TankSourceImage& source) const{
		int w, h;
		SDL_QueryTexture(source.color->get(), nullptr, nullptr, &w, &h);
		
		Texture result(
			renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			w, h
		);
		
		SDL_SetTextureBlendMode(source.color->get(), SDL_ComposeCustomBlendMode(
			SDL_BLENDFACTOR_ZERO,
			SDL_BLENDFACTOR_ONE,
			SDL_BLENDOPERATION_ADD,
			SDL_BLENDFACTOR_ONE,
			SDL_BLENDFACTOR_ZERO,
			SDL_BLENDOPERATION_ADD
		));
		SDL_SetTextureBlendMode(source.light->get(), SDL_ComposeCustomBlendMode(
			SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR,
			SDL_BLENDFACTOR_DST_ALPHA,
			SDL_BLENDOPERATION_ADD,
			SDL_BLENDFACTOR_ONE,
			SDL_BLENDFACTOR_ZERO,
			SDL_BLENDOPERATION_ADD
		));
		
		result.do_with_texture(renderer, [&](){
			fill_texture(renderer);
			SDL_RenderCopy(renderer, source.color->get(), NULL, NULL);
			SDL_RenderCopy(renderer, source.light->get(), NULL, NULL);			
		});
		
		SDL_SetTextureBlendMode(result.get(), SDL_BLENDMODE_BLEND);

		return result;
	}
protected:
	virtual void fill_texture(SDL_Renderer* renderer) const = 0;	
public:
	TankImage get_tank_image(SDL_Renderer* renderer) const{
		return {
			.texture = *this,
			.body = get_image(renderer, BODY),
			.cannon = get_image(renderer, CANNON),
			.gatling = {
				get_image(renderer, GATLING[0]),
				get_image(renderer, GATLING[1]),
				get_image(renderer, GATLING[2]),
			},
			.laser_gun = get_image(renderer, LASER_GUN),
			.thick_cannon = get_image(renderer, THICK_CANNON),
			.launcher = get_image(renderer, LAUNCHER),
			.missile = get_image(renderer, MISSILE),
			.mine_on = get_image(renderer, MINE_ON),
			.mine_off = get_image(renderer, MINE_OFF),
			.ray_gun = {
				get_image(renderer, RAY_GUN[0]),
				get_image(renderer, RAY_GUN[1]),
				get_image(renderer, RAY_GUN[2]),
				get_image(renderer, RAY_GUN[3]),
			},
			.death_ray = get_image(renderer, DEATH_RAY)
		};
	}
};

class SolidColorTexture : public BaseTankTexture {
	const SDL_Color color;
protected:
	void fill_texture(SDL_Renderer* renderer) const{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
		SDL_RenderClear(renderer);
	}
public:
	SolidColorTexture(const SDL_Color& color) : color(color) {}

	SDL_Color random_color() const {
		return color;
	}
};

class TexutedTankTexture : public BaseTankTexture {
	const TextureImage image;
protected:
	void fill_texture(SDL_Renderer* renderer) const{
		SDL_RenderCopy(renderer, image.texture->get(), NULL, NULL);
	}
public:
	TexutedTankTexture(const TextureImage& image) : image(image) {}
	
	SDL_Color random_color() const{
		return image.surface->get_color(
			rand_range(0, image.surface->get()->w),
			rand_range(0, image.surface->get()->h)
		);
	}
};

#define TEXTURE "textures/"

const vector<unique_ptr<TankTexture>> TANK_TEXTURES = [](){
	vector<unique_ptr<TankTexture>> ret;
	ret.push_back(make_unique<SolidColorTexture>(SDL_Color({ .r = 255, .g =   0, .b =   0, .a = 255 })));  // Red
	ret.push_back(make_unique<SolidColorTexture>(SDL_Color({ .r =   0, .g = 255, .b =   0, .a = 255 })));  // Green
	ret.push_back(make_unique<SolidColorTexture>(SDL_Color({ .r =   0, .g =   0, .b = 255, .a = 255 })));  // Blue
	ret.push_back(make_unique<SolidColorTexture>(SDL_Color({ .r = 255, .g = 255, .b =   0, .a = 255 })));  // Yellow
	ret.push_back(make_unique<SolidColorTexture>(SDL_Color({ .r =   0, .g = 255, .b = 255, .a = 255 })));  // Cyan
	ret.push_back(make_unique<SolidColorTexture>(SDL_Color({ .r = 255, .g =   0, .b = 255, .a = 255 })));  // Magenta
	ret.push_back(make_unique<SolidColorTexture>(SDL_Color({ .r = 128, .g = 128, .b = 128, .a = 255 })));  // Gray
	ret.push_back(make_unique<SolidColorTexture>(SDL_Color({ .r = 255, .g = 128, .b =   0, .a = 255 })));  // Orange
	ret.push_back(make_unique<SolidColorTexture>(SDL_Color({ .r = 128, .g =  64, .b =   0, .a = 255 })));  // Brown
	ret.push_back(make_unique<SolidColorTexture>(SDL_Color({ .r = 128, .g = 255, .b =   0, .a = 255 })));  // Lime
	ret.push_back(make_unique<SolidColorTexture>(SDL_Color({ .r = 128, .g =   0, .b = 255, .a = 255 })));  // Purple
	ret.push_back(make_unique<SolidColorTexture>(SDL_Color({ .r =   0, .g = 128, .b =   0, .a = 255 })));  // Dark green
	ret.push_back(make_unique<SolidColorTexture>(SDL_Color({ .r =   0, .g =   0, .b =   0, .a = 255 })));  // Black
	ret.push_back(make_unique<TexutedTankTexture>(register_texture(TEXTURE "army")));
	return ret;
}();

