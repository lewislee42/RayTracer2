# include "World.hpp"

World::World(): displayWindow(800, 600), camera(800, 600) {
	this->vertices = {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}};
}

World::~World() {

}

void World::run() {
	std::cout << "Starting render loop." << std::endl;


	CameraMetadata camData = this->camera.getMetadata();
	Uint32 camDataIndex = displayWindow.createBuffer(&camData, 1, sizeof(CameraMetadata));

	Uint32 vertexBufferIndex = displayWindow.createBuffer((void*)this->vertices.data(), this->vertices.size(), sizeof(Vec3));

	while (1) {
		if (displayWindow.shouldClose()) {
			break;
		}
		displayWindow.updateBuffer(camDataIndex, &camData, 1, sizeof(CameraMetadata));
		displayWindow.render();
	}
}
