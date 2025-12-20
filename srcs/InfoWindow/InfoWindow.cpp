# include "InfoWindow/InfoWindow.hpp"

InfoWindow::InfoWindow(int screenWidth, int screenHeight): screenWidth(screenWidth), screenHeight(screenHeight) {
	InitWindow(screenWidth, screenHeight, "Information Viewport");
	SetTargetFPS(15);
}

InfoWindow::~InfoWindow() {
	CloseWindow();
}

bool InfoWindow::shouldClose() {
	return WindowShouldClose();
}

void InfoWindow::drawWindow() {
	BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText("window text les gooo", 190, 200, 20, LIGHTGRAY);
	EndDrawing();
}


