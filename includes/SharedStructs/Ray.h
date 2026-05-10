#ifndef RAY_H
#define RAY_H

# include "SharedStructs/Vec3.h"

struct Ray {
	point3	origin;
	Vec3	dir;

	Ray() {}
	Ray(const point3 origin, const Vec3 direction): origin(origin), dir(unit_vector(direction)) {

	}
	
	point3 at(float t) const {
		return origin + t * dir;
	}

};

#endif
