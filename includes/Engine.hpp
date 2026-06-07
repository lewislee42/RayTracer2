#pragma once


# include <vector>
# include <iostream>
# include <chrono>
# include <stdexcept>

// # include "entt/entt.hpp"

# include "SDL3/SDL.h"

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlgpu3.h"
#include "imgui_internal.h"

# include "Camera.hpp"

# include "SharedStructs/SharedStructs.h"


class Engine {
public:
	Engine();
	~Engine();


	void run();

	class SDLInitializationError: public std::exception {
		public:
			const char* what() const throw();
	};


private:
	Uint32	screenWidth;
	Uint32	screenHeight;


	std::vector<Triangle>	triangles;


	Camera			camera;
	SDL_Event		event;


	double											deltaTime;
	uint											fps = 0;
	uint 											frameCount = 0;
	std::chrono::high_resolution_clock::time_point	lastTime;


	Uint32						bufferAmount = 0;
	std::vector<SDL_GPUBuffer*>	buffers;
	SDL_GPUTexture*				screenTexture = nullptr;

	Uint32	createBuffer(void* data, size_t elementAmount, size_t elementSize);
	void	updateBuffer(Uint32 bufferIndex, void* data, size_t elementAmount, size_t elementSize);


	SDL_Window*				window		= nullptr;
	SDL_Renderer*			renderer	= nullptr;
	SDL_GPUDevice*			device		= nullptr;
	SDL_GPUComputePipeline*	pipeline	= nullptr;
	
	void	initSDL();
	void	initImGui();
	void	imGuiConfiguration();


	void	render();
	void	sendComputeCommand();
	bool	shouldClose(const SDL_Event& event);


};
