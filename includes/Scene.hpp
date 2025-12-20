#pragma once

// # include "entt/entt.hpp"

# include "InfoWindow/InfoWindow.hpp"
# include "DisplayWindow/DisplayWindow.hpp"

class Scene {

public:
	Scene();
	~Scene();

	void run();
	
private:
	SDL_Event		event;

	InfoWindow		infoWindow;
	DisplayWindow	displayWindow;
};
