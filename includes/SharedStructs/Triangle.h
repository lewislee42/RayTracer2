#ifndef TRIANGLE_H
# define TRIANGLE_H

# include "SharedStructs/Vec3.h"

struct Triangle {
	Vec3	a;
	Vec3	b;
	Vec3	c;

	Triangle() {};
	Triangle(const Vec3 a, const Vec3 b, const Vec3 c): a(a), b(b), c(c) {
	}

};

#endif
