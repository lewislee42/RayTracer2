#ifndef HITRECORD_H
#define HITRECORD_H

# include "SharedStructs/Vec3.h"

struct HitRecord {
	point3	p;
	Vec3	normal;
	float	t;

	HitRecord() {};
	HitRecord(const point3 p, const Vec3 n, const float t): p(p), normal(n), t(t) {}
};

#endif
