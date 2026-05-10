#ifndef VEC_THREE_H
# define VEC_THREE_H

# ifndef SHADER_PROGRAM
	# include <cmath>
	# include <iostream>
# endif


struct Vec3 {
public:
	float e[3];

	Vec3(): e{0.0f, 0.0f, 0.0f} {};
	Vec3(float x, float y, float z): e{x, y, z} {}

	float x() const {return e[0];}
	float y() const {return e[1];}
	float z() const {return e[2];}

	Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
    float operator[](int i) const { return e[i]; }

    float length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

	# ifdef SHADER_PROGRAM
		thread float operator[](int i) { return e[i]; }

		thread Vec3 operator+=(thread const Vec3 v) {
			e[0] += v.e[0];
			e[1] += v.e[1];
			e[2] += v.e[2];
			return *this;
		}

		thread Vec3 operator*=(float t) {
			e[0] *= t;
			e[1] *= t;
			e[2] *= t;
			return *this;
		}

		thread Vec3 operator/=(float t) {
			return *this *= 1/t;
		}
		float length() const {
			return metal::sqrt(length_squared());
		}
	# else
		float& operator[](int i) { return e[i]; }

		Vec3& operator+=(const Vec3& v) {
			e[0] += v.e[0];
			e[1] += v.e[1];
			e[2] += v.e[2];
			return *this;
		}

		Vec3& operator*=(float t) {
			e[0] *= t;
			e[1] *= t;
			e[2] *= t;
			return *this;
		}

		Vec3& operator/=(float t) {
			return *this *= 1/t;
		}

		float length() const {
			return std::sqrt(length_squared());
		}
	# endif
};

# ifdef SHADER_PROGRAM
	inline Vec3 operator+(thread const Vec3 u, thread const Vec3 v) {
		return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
	}

	inline Vec3 operator-(thread const Vec3 u, thread const Vec3 v) {
		return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
	}

	inline Vec3 operator*(thread const Vec3 u, thread const Vec3 v) {
		return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
	}

	inline Vec3 operator*(float t, thread const Vec3 v) {
		return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
	}

	inline Vec3 operator*(thread const Vec3 v, float t) {
		return t * v;
	}

	inline Vec3 operator/(thread const Vec3 v, float t) {
		return (1 / t) * v;
	}

	inline float dot(thread const Vec3 u, thread const Vec3 v) {
		return u.e[0] * v.e[0] \
			 + u.e[1] * v.e[1] \
			 + u.e[2] * v.e[2];
	}

	inline Vec3 cross(thread const Vec3 u, thread const Vec3 v) {
		return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1], \
					u.e[2] * v.e[0] - u.e[0] * v.e[2], \
					u.e[0] * v.e[1] - u.e[1] * v.e[0]);
	}

	inline Vec3 unit_vector(thread const Vec3 v) {
		return v / v.length();
	}
# else 
	inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
		return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
	}

	inline Vec3 operator+(const Vec3& u, const Vec3& v) {
		return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
	}

	inline Vec3 operator-(const Vec3& u, const Vec3& v) {
		return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
	}

	inline Vec3 operator*(const Vec3& u, const Vec3& v) {
		return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
	}

	inline Vec3 operator*(float t, const Vec3& v) {
		return Vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
	}

	inline Vec3 operator*(const Vec3& v, double t) {
		return t * v;
	}

	inline Vec3 operator/(const Vec3& v, double t) {
		return (1/t) * v;
	}

	inline double dot(const Vec3& u, const Vec3& v) {
		return u.e[0] * v.e[0] \
			 + u.e[1] * v.e[1] \
			 + u.e[2] * v.e[2];
	}

	inline Vec3 cross(const Vec3& u, const Vec3& v) {
		return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
					u.e[2] * v.e[0] - u.e[0] * v.e[2],
					u.e[0] * v.e[1] - u.e[1] * v.e[0]);
	}

	inline Vec3 unit_vector(const Vec3& v) {
		return v / v.length();
	}
# endif

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using point3 = Vec3;

#endif
