#ifndef _GAME_ADVANCER_H
#define _GAME_ADVANCER_H

class GameAdvancer{
public:
	virtual void advance() = 0;
	virtual void allow_step() = 0;
};

#endif
