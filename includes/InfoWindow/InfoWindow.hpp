#pragma once

# include "raylib.h"

class InfoWindow {
public:
	InfoWindow(int screenWidth, int screenHeight);
	~InfoWindow();

	bool	shouldClose();
	void	drawWindow();

private:
	int screenWidth;
	int screenHeight;
};
