# include "Engine.hpp"

Engine::Engine(): displayWindow(800, 600), camera(800, 600) {
	this->triangles = {{{0.0f, 0.5f, -1.0f}, {-0.5f, -0.25f, -1.0f}, {0.5f, -0.25f, -2.0f}}};
}

Engine::~Engine() {

}

void Engine::run() {
	std::cout << "Starting render loop." << std::endl;


	CameraMetadata camData = this->camera.getMetadata();
	Uint32 camDataIndex = displayWindow.createBuffer(&camData, 1, sizeof(CameraMetadata));
	Uint32 vertexBufferIndex = displayWindow.createBuffer((void*)this->triangles.data(), this->triangles.size(), sizeof(Triangle));

	while (1) {
		SDL_PollEvent(&this->event);
		ImGui_ImplSDL3_ProcessEvent(&this->event);
		if (displayWindow.shouldClose(event)) {
			break;
		}
		camera.updateMovementBools(event);
		camera.update(displayWindow.deltaTime);
		camData = camera.getMetadata();
		displayWindow.updateBuffer(camDataIndex, &camData, 1, sizeof(CameraMetadata));
		displayWindow.render();
	}
}
