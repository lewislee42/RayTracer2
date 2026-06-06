#pragma once

# include "SDL3/SDL.h"
# include "SharedStructs/SharedStructs.h"
# include "Utils.hpp"


struct MovementBools {
	bool moveForward	= false;
	bool moveBackwards	= false;
	bool moveLeft		= false;
	bool moveRight		= false;

	bool moveUpwards	= false;
	bool moveDownwards	= false;

	bool turnUp			= false;
	bool turnDown		= false;
	bool turnLeft		= false;
	bool turnRight		= false;
};


class Camera {
private:
	point3	center				= Vec3(0, 0, 0);
	Vec3	direction			= Vec3(1, 0, 0);

	float	viewportWidth		= 0.0f;
	float	viewportHeight		= 2.0f;

	float	focalLength			= 1.0f;
	float	vfov				= 90.0f;
	Vec3	vup					= Vec3(0, 1, 0);

	float	moveSpeed			= 1.0f;

	int		screenWidth;
	int		screenHeight;

	Vec3	pixelDeltaU;
	Vec3	pixelDeltaV;
	Vec3	pixel00Loc;

	MovementBools movementBools;

public:
	Camera(const int& screenWidth, const int& screenHeight);

	CameraMetadata	getMetadata() const;
	void			updateCamera();
	void			updateMovementBools(const SDL_Event& event);
	void			updatePosition(const float& deltaTime);
};
