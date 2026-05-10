#pragma once

# include "SharedStructs/SharedStructs.h"

class Camera {
private:
	point3	center				= {0, 0, 0};
	float	viewportWidth		= 0.0f;
	float	viewportHeight		= 2.0f;
	float	focalLength			= 1.0f;

	Vec3	viewportU;
	Vec3	viewportV;

	Vec3	pixelDeltaU;
	Vec3	pixelDeltaV;

	Vec3	viewportUpperLeft;

	Vec3	pixel00Loc;

public:
	Camera(const int& screenWidth, const int& screenHeight);

	CameraMetadata	getMetadata() const;
};
