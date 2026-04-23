# include "DisplayWindow/DisplayWindow.hpp"


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

	this->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_METALLIB, true, NULL);

	if (!SDL_ClaimWindowForGPUDevice(device, window)) {
		SDL_Log("GPU Claim window failed: %s", SDL_GetError());
		throw SDLInitializationError();
	}

	this->lastTime = std::chrono::high_resolution_clock::now();


	size_t shaderSize;
	void* shaderCode = SDL_LoadFile("default.metallib", &shaderSize);

	if (!shaderCode) {
		SDL_Log("Failed to load shader: %s", SDL_GetError());
	}
	SDL_GPUComputePipelineCreateInfo pipelineInfo = {
		.code_size = shaderSize,
		.code = (const Uint8*)shaderCode,
		// .code = (const Uint8*)combinedSource,
		.entrypoint = "computeMain",
		.format = SDL_GPU_SHADERFORMAT_METALLIB,
		.num_readwrite_storage_textures = 1,
		.num_readwrite_storage_buffers = 1,
		.threadcount_x = 64, // have to get kernal local size
		.threadcount_y = 1,
		.threadcount_z = 1
	};
	
	this->pipeline = SDL_CreateGPUComputePipeline(this->device, &pipelineInfo);

	// Creating texture to render to screen
	SDL_GPUTextureCreateInfo texInfo = {
		.type = SDL_GPU_TEXTURETYPE_2D,
		.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, // Standard 32-bit color
		.usage = SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE | SDL_GPU_TEXTUREUSAGE_SAMPLER,
		.width = (Uint32)screenWidth,
		.height = (Uint32)screenHeight,
		.layer_count_or_depth = 1,
		.num_levels = 1
	};
	this->screenTexture = SDL_CreateGPUTexture(device, &texInfo);
}

DisplayWindow::~DisplayWindow() {
	SDL_DestroyGPUDevice(this->device);
	SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void DisplayWindow::render(const std::vector<Vec3>& vertices) {

	// Getting render start time
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

	sendComputeCommand();
	blitTexture();

	// Getting delta time
    std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = endTime - startTime;
    auto differenceMillisecond = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
    long long millisecondCount = differenceMillisecond.count();
	deltaTime = millisecondCount / 1000.0f;
	
	// Getting FPS
	frameCount++;
	std::chrono::duration<double> frameElapsed = endTime - lastTime;
	if (frameElapsed.count() == 1.0) {
        fps = frameCount / frameElapsed.count();

        // Reset for the next infinite cycle
        frameCount = 0;
        lastTime = startTime;
    }
}

void DisplayWindow::sendComputeCommand() {
	// TODO: move transfer buffer creation here and place it under the same command buffer
	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(this->device);
	SDL_GPUStorageTextureReadWriteBinding writeOp = {
		.texture = this->screenTexture,
		.mip_level = 0,
		.layer = 0,
		.cycle = false // Use 'true' if updating every frame
	};
	SDL_GPUStorageBufferReadWriteBinding bufferBind = {
		.buffer = this->vertexBuffer,
		.cycle = false
	};
	SDL_GPUComputePass* pass = SDL_BeginGPUComputePass(
		cmd,
		&writeOp, // texture buffer
		1, // amount of texture buffers
		&bufferBind, // vertexBuffer
		1
	);

	SDL_BindGPUComputePipeline(pass, pipeline);
	SDL_DispatchGPUCompute(pass, this->screenWidth, this->screenHeight, 1);
	SDL_EndGPUComputePass(pass);
	SDL_SubmitGPUCommandBuffer(cmd);
}

void DisplayWindow::blitTexture() {
	// 1. Acquire the Swapchain Texture
	SDL_GPUTexture* swapchainTexture;
	Uint32 w, h;

	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(this->device);

	// Use the Wait version to avoid memory bloat from too many frames in flight
	SDL_WaitAndAcquireGPUSwapchainTexture(cmd, this->window, &swapchainTexture, &w, &h);

	if (swapchainTexture != NULL) {
		// 2. Define the Blit Regions
		SDL_GPUBlitRegion srcRegion = {
			.texture = this->screenTexture,
			.w = this->screenWidth,  // Your compute texture width
			.h = this->screenHeight, // Your compute texture height
		};

		SDL_GPUBlitRegion dstRegion = {
			.texture = swapchainTexture,
			.w = w, // Actual swapchain width
			.h = h, // Actual swapchain height
		};

		// 3. Configure Blit Info
		SDL_GPUBlitInfo blitInfo = {
			.source = srcRegion,
			.destination = dstRegion,
			.load_op = SDL_GPU_LOADOP_DONT_CARE,
			.filter = SDL_GPU_FILTER_NEAREST, // Use LINEAR if scaling
			.cycle = false
		};

		// 4. Execute the Blit (Must be OUTSIDE any Pass)
		SDL_BlitGPUTexture(cmd, &blitInfo);
	}

	// 5. Submit the Command Buffer
	// This automatically presents the swapchain texture to the screen
	SDL_SubmitGPUCommandBuffer(cmd);
}

bool DisplayWindow::shouldClose() {
	SDL_PollEvent(&this->event);
	if (event.type == SDL_EVENT_KEY_DOWN) {
		if (event.key.key == SDLK_ESCAPE)
			return true;
	} else if (event.type == SDL_EVENT_QUIT) {
		return true;
	}
	return false;
}

void DisplayWindow::createVertexBuffer(const std::vector<Vec3>& vertices) {
	Uint32 bufferSize = (Uint32)(vertices.size() * sizeof(Vec3));
	SDL_GPUBufferCreateInfo info = {
		.usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_READ, 
		.size = bufferSize
	};
	this->vertexBuffer = SDL_CreateGPUBuffer(this->device, &info);

	// Creating a transfer buffer to be middle man to vertex buffer and vertices array
	SDL_GPUTransferBufferCreateInfo transferInfo = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = bufferSize // buffer size of vertices
	};
	SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(this->device, &transferInfo); // can be kept around
	

	// Copying content into transfer buffer
	void* mapPtr = SDL_MapGPUTransferBuffer(this->device, transferBuffer, false);
	SDL_memcpy(mapPtr, vertices.data(), bufferSize);
	SDL_UnmapGPUTransferBuffer(this->device, transferBuffer);


	// Set up and run command to copy transfer buffer to vertex buffer
	SDL_GPUCommandBuffer* copyCmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(copyCmd);

	SDL_GPUTransferBufferLocation location = {
		.transfer_buffer = transferBuffer,
		.offset = 0
	};
	SDL_GPUBufferRegion region = {
		.buffer = vertexBuffer,
		.offset = 0,
		.size = bufferSize
	};
	SDL_UploadToGPUBuffer(copyPass, &location, &region, false);

	SDL_EndGPUCopyPass(copyPass);
	SDL_SubmitGPUCommandBuffer(copyCmd);
}



/* ------- Exception ------- */
const char* DisplayWindow::SDLInitializationError::what() const throw() {
	return "Could not initialize SDL components";
}
