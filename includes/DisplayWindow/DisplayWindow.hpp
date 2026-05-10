#pragma once

# include "Metal/Metal.hpp"
# include "Foundation/Foundation.hpp"
# include "QuartzCore/QuartzCore.hpp"
# include "SDL3/SDL.h"

# include <chrono>
# include <stdexcept>

# include "SharedStructs/SharedStructs.h"

class DisplayWindow {
public:
	DisplayWindow(int screenWidth, int screenHeight);
	~DisplayWindow();


	uint	deltaTime;
	uint	fps = 0;


	void	render();
	bool	shouldClose();

	Uint32	createBuffer(void* data, size_t elementAmount, size_t elementSize);
	void	updateBuffer(Uint32 bufferIndex, void* data, size_t elementAmount, size_t elementSize);

	class SDLInitializationError: public std::exception {
		public:
			const char* what() const throw();
	};

private:
	Uint32	screenWidth;
	Uint32	screenHeight;
	Uint32	bufferAmount = 0;

	uint 											frameCount = 0;
	std::chrono::high_resolution_clock::time_point	lastTime;

	SDL_Window*				window		= nullptr;
	SDL_Renderer*			renderer	= nullptr;
	SDL_GPUDevice*			device;
	SDL_GPUComputePipeline*	pipeline;

	std::vector<SDL_GPUBuffer*>	buffers;
	SDL_GPUTexture*				screenTexture = nullptr;

	SDL_Event				event;
	
	void	sendComputeCommand();
	void	blitTexture();
};
