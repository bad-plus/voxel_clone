#pragma once
#include "window/window.h"
#include "input/input.h"
#include "../render/render.h"
#include "../render/camera.hpp"
#include "input/input_handler.h"
#include "../utils/resource/resources.h"
#include "../utils/resource/loader.h"
#include "../world/world.h"
#include "../world/generation/world_generator.h"

class Game;

struct GameSystemInfo {
	double update_tick_time;
	double render_time;
	double chunk_creation_time;
};

class Game {
public:
	Game();
	~Game();
	void quit();
	GameSystemInfo getSystemInfo();
	GameSystemInfo m_game_system_info;

private:
	void initGLFW();
	void startMainLoop();
	void worldGenerationThread();
	void worldUpdaterThread();
	void movementUpdaterThread();

	std::atomic<bool> m_quit{ false };
	std::vector<std::thread> m_threads;

	std::unique_ptr<Window> m_window;
	std::unique_ptr<Input> m_input;
	std::unique_ptr<Render> m_render;
	std::unique_ptr<InputHandler> m_input_handler;
	std::unique_ptr<Resources> m_resources;
	std::unique_ptr<Loader> m_loader;
	std::unique_ptr<World> m_world;
	std::unique_ptr<WorldGenerator> m_world_generator;
};