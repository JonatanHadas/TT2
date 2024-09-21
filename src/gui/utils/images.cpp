#include "images.h"

#include <SDL_image.h>

#include <map>
#include <functional>

class Surface{
	SDL_Surface* surface;
public:
	Surface(int w, int h, const SDL_PixelFormat& format) : surface(SDL_CreateRGBSurface(
		0, w, h, format.BitsPerPixel,
		format.Rmask, format.Gmask, format.Bmask, format.Amask
	)) {}
	explicit Surface(const string& path) : surface(IMG_Load(path.c_str())) {}
	
	Surface(const Surface&) = delete;
	Surface(Surface&& other) : surface(other.surface) {
		other.surface = nullptr;
	}
	
	Surface& operator=(const Surface&) = delete;
	Surface& operator=(Surface&& other){
		surface = other.surface;
		other.surface = nullptr;
		return *this;
	}
	
	~Surface(){
		if(surface != nullptr) SDL_FreeSurface(surface); 
	}
	
	SDL_Surface* get() const{
		return surface;
	}
};

class SurfaceLock{
	Surface* surface;
public:
	explicit SurfaceLock(Surface& surface) : surface(&surface) {
		if(surface.get() != nullptr) {
			if(SDL_LockSurface(surface.get()) != 0){
				this->surface = nullptr;
			}
		}
	}
	
	bool check() const{
		return surface != nullptr;
	}
	
	SurfaceLock(const SurfaceLock&) = delete;
	SurfaceLock(SurfaceLock&& other) : surface(other.surface) {
		other.surface = nullptr;
	}
	
	SurfaceLock& operator=(const SurfaceLock&) = delete;
	SurfaceLock& operator=(SurfaceLock&& other){
		surface = other.surface;
		other.surface = nullptr;
		return *this;
	}
	
	~SurfaceLock(){
		if(surface != nullptr && surface->get() != nullptr) SDL_UnlockSurface(surface->get());
	}
};

map<string, map<filter_t, unique_ptr<Texture>>>& get_images(){
	static map<string, map<filter_t, unique_ptr<Texture>>> images;
	return images;
}

const unique_ptr<Texture>& register_image(const string& name, filter_t filter){
	if(get_images().count(name) == 0) get_images().insert({name, map<filter_t, unique_ptr<Texture>>()});
	if(get_images()[name].count(filter) == 0) get_images()[name].insert({filter, nullptr});
	return get_images()[name][filter];
}

void free_images(){
	for(auto& [name, textures]: get_images())
		for(auto& [filter, texture]: textures)
			texture = nullptr;
}

static inline Uint32& pixel(SDL_Surface* surface, int x, int y){
	return *((Uint32*)((Uint8*)(surface->pixels) + (y * surface->pitch) + (x * surface->format->BytesPerPixel)));
}

unique_ptr<Texture> load_image(SDL_Renderer* renderer, const Surface& surface, filter_t filter){
	if(filter == nullptr) return make_unique<Texture>(renderer, surface.get());
	
	Surface new_surface(surface.get()->w, surface.get()->h, *surface.get()->format);
	if(new_surface.get() == nullptr) return nullptr;
	
	{
		SurfaceLock lock(new_surface);
		if(!lock.check()) return nullptr;

		for(int x = 0; x < surface.get()->w; x++){
			for(int y = 0; y < surface.get()->h; y++){
				SDL_Color color;
				SDL_GetRGBA(
					pixel(surface.get(), x, y),
					surface.get()->format,
					&color.r, &color.g, &color.b, &color.a
				);
				color = filter(color);
				pixel(new_surface.get(), x, y) = SDL_MapRGBA(
					new_surface.get()->format,
					color.r, color.g, color.b, color.a
				);
			}
		}
	}			
	
	return make_unique<Texture>(renderer, new_surface.get());
}

bool load_images(SDL_Renderer* renderer, const string& name, map<filter_t, unique_ptr<Texture>>& textures){
	Surface surface("data/images/" + name + ".png");
	if(surface.get() == nullptr) return false;

	for(auto& [filter, texture]: textures){
		texture = load_image(renderer, surface, filter);
		if(texture == nullptr || texture->get() == nullptr){
			return false;
		}
	}
	
	return true;
}

bool load_images(SDL_Renderer* renderer){
	atexit(free_images);
	
	for(auto& [name, textures]: get_images()){
		if(!load_images(renderer, name, textures)) return false;
	}
	
	return true;
}
