#ifndef _IMAGES_H
#define _IMAGES_H

#include "utils.h"

#include <string>
#include <memory>

class Surface{
	SDL_Surface* surface;
	Uint32& get_pixel(int x, int y) const;
public:
	Surface(int w, int h, const SDL_PixelFormat& format);
	explicit Surface(const string& path);
	
	Surface(const Surface&) = delete;
	Surface(Surface&& other);
	
	Surface& operator=(const Surface&) = delete;
	Surface& operator=(Surface&& other);
	
	~Surface();
	
	SDL_Surface* get() const;
	
	SDL_Color get_color(int x, int y) const;
	void set_color(int x, int y, SDL_Color color);
};

class SurfaceLock{
	Surface* surface;
public:
	explicit SurfaceLock(Surface& surface);
	
	bool check() const;
	
	SurfaceLock(const SurfaceLock&) = delete;
	SurfaceLock(SurfaceLock&& other);
	
	SurfaceLock& operator=(const SurfaceLock&) = delete;
	SurfaceLock& operator=(SurfaceLock&& other);
	
	~SurfaceLock();
};

using namespace std;

typedef SDL_Color(*filter_t)(const SDL_Color&);

const unique_ptr<Texture>& register_image(const string& name, filter_t filter = nullptr);

struct TextureImage{
	const unique_ptr<Texture>& texture;
	const unique_ptr<Surface>& surface;
};

const TextureImage register_texture(const string& name);

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
