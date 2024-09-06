#include "colors.h"

SDL_Color bg_color = {255, 255, 255, 255};
SDL_Color board_color = {240, 240, 240, 255};
SDL_Color wall_color = {150, 150, 150, 255};

vector<SDL_Color> tank_colors({
	{255, 0, 0, 255},
	{0, 255, 0, 255},
	{0, 0, 255, 255}
});
