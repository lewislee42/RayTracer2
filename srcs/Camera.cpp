# include "Camera.hpp"

Camera::Camera(const int& screenWidth, const int& screenHeight) {
	// Screen Width/Height setting
	this->screenWidth	= screenWidth;
	this->screenHeight	= screenHeight;


	// Viewport setting/calculation
	float theta				= degreesToRadians(vfov);
    float h					= std::tan(theta / 2);
	this->viewportHeight	= 2.0f * h * focalLength;
	this->viewportWidth		= viewportHeight * (float(screenWidth) / screenHeight);


	updateCamera();
}

void Camera::updateCamera() {
	// Viewport calculation
	Vec3 w			= unitVector(-direction);
	Vec3 u			= unitVector(cross(vup, w));
	Vec3 v 			= cross(w, u);
	Vec3 viewportU	= viewportWidth * u;
	Vec3 viewportV	= viewportHeight * -v;

	// Pixel position calculation
	this->pixelDeltaU = viewportU / this->screenWidth;
	this->pixelDeltaV = viewportV / this->screenHeight;

    Vec3 viewportUpperLeft = center - (focalLength * w) - viewportU / 2 - viewportV / 2;
	this->pixel00Loc = viewportUpperLeft + 0.5f * (pixelDeltaU + pixelDeltaV);
}

CameraMetadata Camera::getMetadata() const {
	return {
		this->pixelDeltaU,
		this->pixelDeltaV,
		this->pixel00Loc,
		this->center,
	};
}

void Camera::updateMovementBools(const SDL_Event& event) {
	if (event.type == SDL_EVENT_KEY_DOWN) {
		switch(event.key.key) {
			case SDLK_W:
				movementBools.moveForward = true;
				break;
			case SDLK_S:
				movementBools.moveBackwards = true;
				break;
			case SDLK_A:
				movementBools.moveLeft = true;
				break;
			case SDLK_D:
				movementBools.moveRight = true;
				break;
			case SDLK_LEFT:
				movementBools.turnLeft = true;
				break;
			case SDLK_RIGHT:
				movementBools.turnRight = true;
				break;
			case SDLK_UP:
				movementBools.turnUp = true;
				break;
			case SDLK_DOWN:
				movementBools.turnDown = true;
				break;
			case SDLK_SPACE:
				movementBools.moveUpwards = true;
				break;
			case SDLK_LCTRL:
				movementBools.moveDownwards = true;
				break;
		}
	}
	else if (event.type == SDL_EVENT_KEY_UP) {
		switch(event.key.key) {
			case SDLK_W:
				movementBools.moveForward = false;
				break;
			case SDLK_S:
				movementBools.moveBackwards = false;
				break;
			case SDLK_A:
				movementBools.moveLeft = false;
				break;
			case SDLK_D:
				movementBools.moveRight = false;
				break;
			case SDLK_LEFT:
				movementBools.turnLeft = false;
				break;
			case SDLK_RIGHT:
				movementBools.turnRight = false;
				break;
			case SDLK_UP:
				movementBools.turnUp = false;
				break;
			case SDLK_DOWN:
				movementBools.turnDown = false;
				break;
			case SDLK_SPACE:
				movementBools.moveUpwards = false;
				break;
			case SDLK_LCTRL:
				movementBools.moveDownwards = false;
				break;	
		}
	}
}

void	Camera::updatePosition(const float& deltaTime) {
	// std::cout << "deltaTime: " << std::to_string(deltaTime) << std::endl;
	if (movementBools.moveForward == true) {
		center = center + (direction * moveSpeed * deltaTime);
		std::cout << "W pressed... center: " << center << std::endl;
	}
	if (movementBools.moveBackwards == true) {
		center = center + (-direction * moveSpeed * deltaTime);
		std::cout << "S pressed... center: " << center << std::endl;
	}
	if (movementBools.moveLeft == true) {
		center = center + (-1 * cross(direction, vup) * moveSpeed * deltaTime);
		std::cout << "A pressed... center: " << center << std::endl;
	}
	if (movementBools.moveRight == true) {
		center = center + (cross(direction, vup) * moveSpeed * deltaTime);
		std::cout << "D pressed... center: " << center << std::endl;
	}
}
