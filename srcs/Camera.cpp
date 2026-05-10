# include "Camera.hpp"

Camera::Camera(const int& screenWidth, const int& screenHeight) {

	this->viewportWidth = viewportHeight * (float(screenWidth) / screenHeight);

	this->viewportU = Vec3(viewportWidth, 0, 0);
	this->viewportV = Vec3(0, -viewportHeight, 0);

	// Calculate pixel delta
	this->pixelDeltaU = viewportU / screenWidth;
	this->pixelDeltaV = viewportV / screenHeight;

	
	this->viewportUpperLeft = center - Vec3(0, 0, focalLength) - (viewportU / 2) - (viewportV / 2);
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
