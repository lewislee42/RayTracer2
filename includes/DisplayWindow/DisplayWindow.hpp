#pragma once

# include "Metal/Metal.hpp"
# include "Foundation/Foundation.hpp"
# include "QuartzCore/QuartzCore.hpp"
# include "SDL3/SDL.h"

# include <chrono>
# include <stdexcept>

# include "SharedStructs.h"

class DisplayWindow {
public:
	DisplayWindow(int screenWidth, int screenHeight);
	~DisplayWindow();


	uint	deltaTime;
	uint	fps = 0;


	void	render(const std::vector<Vec3>& vertices);
	bool	shouldClose();
	void	createVertexBuffer(const std::vector<Vec3>& vertices);

	class SDLInitializationError: public std::exception {
		public:
			const char* what() const throw();
	};

private:
	Uint32 screenWidth;
	Uint32 screenHeight;

	uint 											frameCount = 0;
	std::chrono::high_resolution_clock::time_point	lastTime;

	SDL_Window*				window		= nullptr;
	SDL_Renderer*			renderer	= nullptr;
	SDL_GPUDevice*			device;
	SDL_GPUComputePipeline*	pipeline;

	SDL_GPUTransferBuffer*	transferBuffer = nullptr;
	SDL_GPUBuffer*			vertexBuffer = nullptr;
	SDL_GPUTexture*			screenTexture = nullptr;

	SDL_Event				event;
	
	void	sendComputeCommand();
	void	blitTexture();
};
