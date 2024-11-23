#ifndef _GAME_OBSERVER_H
#define _GAME_OBSERVER_H

#include "../data/game_objects.h"

class GameObserver{
public:
	virtual void on_shot_removed(int shot_id) {};
	virtual void on_missile_removed(int missile_id) {};
	virtual void on_death_ray_removed(int death_ray_id) {};
};

#endif
