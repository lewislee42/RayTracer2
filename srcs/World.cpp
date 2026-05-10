# include "World.hpp"

World::World(): displayWindow(800, 600), camera(800, 600) {
	this->triangles = {{{0.0f, 0.5f, -1.0f}, {-0.5f, -0.25f, -1.0f}, {0.5f, -0.25f, -2.0f}}};
}

World::~World() {

}

void World::run() {
	std::cout << "Starting render loop." << std::endl;


	CameraMetadata camData = this->camera.getMetadata();
	Uint32 camDataIndex = displayWindow.createBuffer(&camData, 1, sizeof(CameraMetadata));

	Uint32 vertexBufferIndex = displayWindow.createBuffer((void*)this->triangles.data(), this->triangles.size(), sizeof(Triangle));

	while (1) {
		if (displayWindow.shouldClose()) {
			break;
		}
		displayWindow.updateBuffer(camDataIndex, &camData, 1, sizeof(CameraMetadata));
		displayWindow.render();
	}
}
