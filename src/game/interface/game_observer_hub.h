#ifndef _GAME_OBSERVER_HUB_H
#define _GAME_OBSERVER_HUB_H

#include "game_observer.h"

#include <set>

using namespace std;

class GameObserverHub : public GameObserver {
	set<GameObserver*> observers;
public:
	GameObserverHub();
	
	void add_observer(GameObserver* observer);
	void remove_observer(GameObserver* observer);
	
	void on_shot_removed(int shot_id);	
	void on_missile_removed(int missile_id);
	void on_death_ray_removed(int death_ray_id);
};

#endif
