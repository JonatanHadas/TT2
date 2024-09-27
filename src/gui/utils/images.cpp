#include "images.h"

#include <SDL_image.h>

#include <map>
#include <functional>

Surface::Surface(int w, int h, const SDL_PixelFormat& format) : surface(SDL_CreateRGBSurface(
	0, w, h, format.BitsPerPixel,
	format.Rmask, format.Gmask, format.Bmask, format.Amask
)) {}
Surface::Surface(const string& path) : surface(IMG_Load(path.c_str())) {}

Surface::Surface(Surface&& other) : surface(other.surface) {
	other.surface = nullptr;
}

Surface& Surface::operator=(Surface&& other){
	surface = other.surface;
	other.surface = nullptr;
	return *this;
}

Surface::~Surface(){
	if(surface != nullptr) SDL_FreeSurface(surface); 
}

SDL_Surface* Surface::get() const{
	return surface;
}

Uint32& Surface::get_pixel(int x, int y) const{
	return *((Uint32*)(
		((Uint8*)(surface->pixels))
		+ (y * surface->pitch)
		+ (x * surface->format->BytesPerPixel)
	));
}
SDL_Color Surface::get_color(int x, int y) const{
	SDL_Color color;
	SDL_GetRGBA(
		get_pixel(x, y), surface->format,
		&color.r, &color.g, &color.b, &color.a
	);
	return color;
}
void Surface::set_color(int x, int y, SDL_Color color){
	get_pixel(x, y) = SDL_MapRGBA(
		surface->format,
		color.r, color.g, color.b, color.a
	);
}


SurfaceLock::SurfaceLock(Surface& surface) : surface(&surface) {
	if(surface.get() != nullptr) {
		if(SDL_LockSurface(surface.get()) != 0){
			this->surface = nullptr;
		}
	}
}

bool SurfaceLock::check() const{
	return surface != nullptr;
}

SurfaceLock::SurfaceLock(SurfaceLock&& other) : surface(other.surface) {
	other.surface = nullptr;
}

SurfaceLock& SurfaceLock::operator=(SurfaceLock&& other){
	surface = other.surface;
	other.surface = nullptr;
	return *this;
}

SurfaceLock::~SurfaceLock(){
	if(surface != nullptr && surface->get() != nullptr) SDL_UnlockSurface(surface->get());
}

map<string, map<filter_t, unique_ptr<Texture>>>& get_images(){
	static map<string, map<filter_t, unique_ptr<Texture>>> images;
	return images;
}

map<string, unique_ptr<Surface>>& get_surfaces(){
	static map<string, unique_ptr<Surface>> surfaces;
	return surfaces;
}

const unique_ptr<Texture>& register_image(const string& name, filter_t filter){
	if(get_images().count(name) == 0) get_images().insert({name, map<filter_t, unique_ptr<Texture>>()});
	if(get_images()[name].count(filter) == 0) get_images()[name].insert({filter, nullptr});
	return get_images()[name][filter];
}

const TextureImage register_texture(const string& name){
	if(get_surfaces().count(name) == 0) get_surfaces().insert({name, nullptr});
	return {
		.texture = register_image(name),
		.surface = get_surfaces()[name]
	};
}

void free_images(){
	for(auto& [name, textures]: get_images())
		for(auto& [filter, texture]: textures)
			texture = nullptr;
	for(auto& [name, surface]: get_surfaces())
		surface = nullptr;
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
				new_surface.set_color(x, y, filter(surface.get_color(x, y)));
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
	
	if(get_surfaces().count(name) > 0){
		get_surfaces()[name] = make_unique<Surface>(move(surface));
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
