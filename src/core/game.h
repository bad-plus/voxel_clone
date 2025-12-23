#pragma once
#include <vector>
#include <memory>
#include <thread>

struct Window;
struct Input;
struct Render;
struct InputHandler;
struct Resources;
struct Loader;
struct World;
struct WorldGenerator;
struct UI;
struct DebugOverlay;

#include "input/input.h"

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
	GameSystemInfo getSystemInfo() const;
	GameSystemInfo m_game_system_info;

	void run();
private:
	void initGLFW() const;
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
	std::unique_ptr<UI> m_ui;
	std::unique_ptr<DebugOverlay> m_debug_overlay;
};