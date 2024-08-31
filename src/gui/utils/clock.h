#ifndef _CLOCK_H
#define _CLOCK_H

class Clock{
	int last_tick;
	double remainder;
public:
	Clock();
	void tick(double length);
};

#endif
