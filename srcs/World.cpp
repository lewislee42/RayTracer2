# include "World.hpp"

World::World(): displayWindow(800, 600) {
	this->vertices = {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}};
}

World::~World() {

}

void World::run() {
	std::cout << "Starting render loop." << std::endl;
	displayWindow.createVertexBuffer(this->vertices);
	while (1) {
		if (displayWindow.shouldClose()) {
			break;
		}
		displayWindow.render(vertices);
	}
}
