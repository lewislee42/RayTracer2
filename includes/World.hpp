#pragma once


# include <vector>
# include <iostream>

// # include "entt/entt.hpp"

# include "DisplayWindow/DisplayWindow.hpp"

# include "SharedStructs.h"


class World {

public:
	World();
	~World();

	std::vector<Vec3>	vertices;

	void run();
	
private:
	DisplayWindow	displayWindow;
};
