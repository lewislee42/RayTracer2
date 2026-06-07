# include "Engine.hpp"

Engine::Engine(): camera(800, 600) {
	this->screenWidth= 800;
	this->screenHeight = 600;
	initSDL();
	initImGui();

	this->lastTime = std::chrono::high_resolution_clock::now();
	this->triangles = {{{0.0f, 0.5f, -1.0f}, {-0.5f, -0.25f, -1.0f}, {0.5f, -0.25f, -2.0f}}};
}

Engine::~Engine() {
	SDL_WaitForGPUIdle(device);
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui::DestroyContext();

	SDL_ReleaseWindowFromGPUDevice(this->device, window);
	SDL_DestroyGPUDevice(this->device);
	SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void Engine::run() {
	std::cout << "Starting render loop." << std::endl;


	CameraMetadata camData = this->camera.getMetadata();
	Uint32 camDataIndex = createBuffer(&camData, 1, sizeof(CameraMetadata));
	Uint32 vertexBufferIndex = createBuffer((void*)this->triangles.data(), this->triangles.size(), sizeof(Triangle));

	while (1) {
		SDL_PollEvent(&this->event);
		ImGui_ImplSDL3_ProcessEvent(&this->event);
		if (shouldClose(event)) {
			break;
		}
		camera.updateMovementBools(event);
		camera.update(deltaTime);
		camData = camera.getMetadata();
		updateBuffer(camDataIndex, &camData, 1, sizeof(CameraMetadata));
		render();
	}
}

void	Engine::initSDL() {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		throw SDLInitializationError();
	}

	/*
	 *  Initializes SDL window and renderer
	 */
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

	/*
	 *  Initializes SDL GPU device for shader compilation & communication
	 */
	this->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_METALLIB, true, NULL);
	if (!SDL_ClaimWindowForGPUDevice(device, window)) {
		SDL_Log("GPU Claim window failed: %s", SDL_GetError());
		throw SDLInitializationError();
	}

	/*
	 *  Compiles shader and loads it as a pipeline
	 */
	size_t shaderSize;
	// Come back to this to make this dynamic
	void* shaderCode = SDL_LoadFile("/Users/lewislee/Documents/Coding/graphicProjects/blackhole-sim/default.metallib", &shaderSize);
	if (!shaderCode) {
		SDL_Log("Failed to load shader: %s", SDL_GetError());
		throw SDLInitializationError();
	}
	SDL_GPUComputePipelineCreateInfo pipelineInfo = {
		.code_size = shaderSize,
		.code = (const Uint8*)shaderCode,
		.entrypoint = "computeMain",
		.format = SDL_GPU_SHADERFORMAT_METALLIB,
		.num_readwrite_storage_textures = 1,
		.num_readwrite_storage_buffers = 2,
		.threadcount_x = 16,
		.threadcount_y = 16,
		.threadcount_z = 1
	};
	this->pipeline = SDL_CreateGPUComputePipeline(this->device, &pipelineInfo);

	/*
	 *  Creates texture to render for the screen
	 */
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

void	Engine::initImGui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL3_InitForSDLGPU(window);
	ImGui_ImplSDLGPU3_InitInfo initInfo = {};
	initInfo.Device = this->device;
	initInfo.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(this->device, this->window);
	initInfo.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
	initInfo.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
	initInfo.PresentMode = SDL_GPU_PRESENTMODE_VSYNC;
	ImGui_ImplSDLGPU3_Init(&initInfo);
}



void Engine::render() {
	imGuiConfiguration();

	// Getting render start time
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

	sendComputeCommand();

	// Getting delta time
    std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = endTime - startTime;
	deltaTime = elapsed.count();
	
	// Getting FPS
	frameCount++;
	std::chrono::duration<double> frameElapsed = endTime - lastTime;
	if (frameElapsed.count() >= 1.0) {
        fps = frameCount / frameElapsed.count();

        // Reset for the next infinite cycle
        frameCount = 0;
        lastTime = startTime;
    }
}

void	Engine::imGuiConfiguration() {
	ImGui_ImplSDLGPU3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// Create a dockspace in main viewport.
	ImGuiID dockspace_id = ImGui::GetID("My Dockspace");
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	// Create settings
	if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr)
	{
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
		ImGuiID dock_id_left = 0;
		ImGuiID dock_id_main = dockspace_id;
		ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Left, 0.20f, &dock_id_left, &dock_id_main);
		ImGuiID dock_id_left_top = 0;
		ImGuiID dock_id_left_bottom = 0;
		ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Up, 0.50f, &dock_id_left_top, &dock_id_left_bottom);
		ImGui::DockBuilderDockWindow("Main Viewport", dock_id_main);
		ImGui::DockBuilderDockWindow("Properties", dock_id_left_top);
		ImGui::DockBuilderDockWindow("Scene", dock_id_left_bottom);
		ImGui::DockBuilderFinish(dockspace_id);
	}

	// Submit dockspace
	ImGui::DockSpaceOverViewport(dockspace_id, viewport, ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::Begin("Main Viewport");
	ImGui::Image((ImTextureID)screenTexture, ImVec2((float)screenWidth, (float)screenHeight));
	ImGui::End();

	ImGui::Begin("Properties");
	ImGui::Text("FPS: %d", fps);
	ImGui::End();

	ImGui::Begin("Scene");
	ImGui::End();
}


void Engine::sendComputeCommand() {
	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();

	/*
	 *	Running compute shader
	 */
	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(this->device);
	SDL_GPUStorageTextureReadWriteBinding writeOp = {
		.texture = this->screenTexture,
		.mip_level = 0,
		.layer = 0,
		.cycle = false // Use 'true' if updating every frame
	};

	std::vector<SDL_GPUStorageBufferReadWriteBinding> bufferBinds;
	for (int i = 0; i < this->buffers.size(); i++) {
		bufferBinds.push_back(
			{
				.buffer = this->buffers[i],
				.cycle =false 
			}
		);
	}

	SDL_GPUComputePass* pass = SDL_BeginGPUComputePass(
		cmd,
		&writeOp, // texture buffer
		1, // amount of texture buffers
		bufferBinds.data(),
		bufferBinds.size()
	);

	SDL_BindGPUComputePipeline(pass, pipeline);
	SDL_DispatchGPUCompute(pass, this->screenWidth, this->screenHeight, 1);
	SDL_EndGPUComputePass(pass);


	/*
	 *	Renders ImGui
	 */
	SDL_GPUTexture *swapchainTexture;
	SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swapchainTexture, nullptr, nullptr); // Acquire a swapchain texture

	if (swapchainTexture != nullptr) {

		ImGui_ImplSDLGPU3_PrepareDrawData(drawData, cmd);
		// Setup and start a render pass
		SDL_GPUColorTargetInfo targetInfo = {};
		targetInfo.texture = swapchainTexture;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		targetInfo.clear_color = SDL_FColor { clear_color.x, clear_color.y, clear_color.z, clear_color.w };
		targetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
		targetInfo.store_op = SDL_GPU_STOREOP_STORE;
		targetInfo.mip_level = 0;
		targetInfo.layer_or_depth_plane = 0;
		targetInfo.cycle = false;
		SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmd, &targetInfo, 1, nullptr);

		// Render ImGui
		ImGui_ImplSDLGPU3_RenderDrawData(drawData, cmd, renderPass);

		SDL_EndGPURenderPass(renderPass);
	}


	SDL_SubmitGPUCommandBuffer(cmd);
}



bool Engine::shouldClose(const SDL_Event& event) {
	if (event.type == SDL_EVENT_KEY_DOWN) {
		if (event.key.key == SDLK_ESCAPE)
			return true;
	} else if (event.type == SDL_EVENT_QUIT) {
		return true;
	}
	return false;
}


Uint32 Engine::createBuffer(void* data, size_t elementAmount, size_t elementSize) {
	Uint32 bufferSize = (Uint32)(elementAmount * elementSize);
	SDL_GPUBufferCreateInfo info = {
		.usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_READ, 
		.size = bufferSize
	};
	this->buffers.push_back(SDL_CreateGPUBuffer(this->device, &info));
	this->bufferAmount++;
	updateBuffer(this->bufferAmount - 1, data, elementAmount, elementSize);
	return this->bufferAmount - 1;
}


void Engine::updateBuffer(Uint32 bufferIndex, void* data, size_t elementAmount, size_t elementSize) {
	Uint32 bufferSize = (Uint32)(elementAmount * elementSize);
	// Creating a transfer buffer to be middle man to vertex buffer and vertices array
	SDL_GPUTransferBufferCreateInfo transferInfo = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = bufferSize // buffer size of vertices
	};
	SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(this->device, &transferInfo);
	

	// Copying content into transfer buffer
	void* mapPtr = SDL_MapGPUTransferBuffer(this->device, transferBuffer, false);
	SDL_memcpy(mapPtr, data, bufferSize);
	SDL_UnmapGPUTransferBuffer(this->device, transferBuffer);


	// Set up and run command to copy transfer buffer to vertex buffer
	SDL_GPUCommandBuffer* copyCmd = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(copyCmd);


	SDL_GPUTransferBufferLocation location = {
		.transfer_buffer = transferBuffer,
		.offset = 0
	};
	SDL_GPUBufferRegion region = {
		.buffer = this->buffers[bufferIndex],
		.offset = 0,
		.size = bufferSize
	};
	SDL_UploadToGPUBuffer(copyPass, &location, &region, false);


	SDL_EndGPUCopyPass(copyPass);
	SDL_SubmitGPUCommandBuffer(copyCmd);
}



/* ------- Exception ------- */
const char* Engine::SDLInitializationError::what() const throw() {
	return "Could not initialize SDL components";
}

