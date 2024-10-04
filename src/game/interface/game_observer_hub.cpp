#include "game_observer_hub.h"

GameObserverHub::GameObserverHub(){}

void GameObserverHub::add_observer(GameObserver* observer){
	observers.insert(observer);
}
void GameObserverHub::remove_observer(GameObserver* observer){
	observers.erase(observer);
}

void GameObserverHub::on_shot_removed(int shot_id){
	for(auto observer: observers) observer->on_shot_removed(shot_id);
}
