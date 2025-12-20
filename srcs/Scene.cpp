# include "Scene.hpp"
#include "SDL3/SDL_events.h"

Scene::Scene(): infoWindow(400, 300), displayWindow(800, 600) {
}

Scene::~Scene() {

}

void Scene::run() {
	while (1) {
		SDL_PollEvent(&this->event);
		if (this->event.type == SDL_EVENT_QUIT || infoWindow.shouldClose()) {
			break;
		}
		infoWindow.drawWindow();
		displayWindow.drawWindow();
	}
}
