# include "DisplayWindow/DisplayWindow.hpp"

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

DisplayWindow::DisplayWindow(int screenWidth, int screenHeight):
	screenWidth(screenWidth),
	screenHeight(screenHeight)
{

	// Use SDL window creation and input handling
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		throw SDLInitializationError();
	}

	if (!SDL_CreateWindowAndRenderer(
			"Viewport Display", 
			this->screenWidth, 
			this->screenHeight, 
			SDL_WINDOW_MAXIMIZED, 
			&this->window, 
			&this->renderer
		)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
		throw SDLInitializationError();
	}

	
}

DisplayWindow::~DisplayWindow() {
	SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void DisplayWindow::drawWindow() {
	
}



/* ------- Exception ------- */
const char* DisplayWindow::SDLInitializationError::what() const throw() {
	return "Could not initialize SDL components";
}
