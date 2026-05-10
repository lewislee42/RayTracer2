#pragma once


# include <vector>
# include <iostream>

// # include "entt/entt.hpp"

# include "DisplayWindow/DisplayWindow.hpp"
# include "Camera.hpp"

# include "SharedStructs/SharedStructs.h"


class World {

public:
	World();
	~World();

	std::vector<Triangle>	triangles;

	void run();
	
private:
	DisplayWindow	displayWindow;
	Camera			camera;
};
