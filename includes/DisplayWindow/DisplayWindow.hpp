#pragma once

# include "Metal/Metal.hpp"
# include "Foundation/Foundation.hpp"
# include "QuartzCore/QuartzCore.hpp"
# include "SDL3/SDL.h"

class DisplayWindow {

	public:
		DisplayWindow(int screenWidth, int screenHeight);
		~DisplayWindow();

		void	drawWindow();

		class SDLInitializationError: public std::exception {
			public:
				const char* what() const throw();
		};

	private:
		int screenWidth;
		int screenHeight;

		SDL_Window*		window		= nullptr;
		SDL_Renderer*	renderer	= nullptr;
};
