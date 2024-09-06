#ifndef _PLAYER_INTERFACE_H
#define _PLAYER_INTERFACE_H

#include "../data/game_objects.h"

class PlayerInterface{
public:
	virtual void step(int round, KeyState key_state) = 0;
	virtual void set_active(bool active) = 0;
};

#endif
