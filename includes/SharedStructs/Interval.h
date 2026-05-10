#ifndef INTERVAL_H
#define INTERVAL_H

struct Interval {
	float	min;
	float	max;

	Interval() {};
	Interval(const float min, const float max): min(min), max(max) {}
};

#endif
