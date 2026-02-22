#pragma once
#include <vector>
#include <memory>
#include <thread>
#include <core/time.hpp>

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
struct Client;

struct GameSystemInfo {
	Time update_tick_time;
	Time render_time;
	Time chunk_creation_time;
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

	void worldUpdaterThread();
	void movementUpdaterThread();

	void initSystems();

	std::atomic<bool> m_quit{ false };
	std::vector<std::thread> m_threads;

	std::unique_ptr<Window> m_window;
	std::unique_ptr<Input> m_input;
	std::unique_ptr<Render> m_render;
	std::unique_ptr<InputHandler> m_input_handler;
	std::unique_ptr<Resources> m_resources;
	std::unique_ptr<Loader> m_loader;
	std::unique_ptr<UI> m_ui;
	std::unique_ptr<DebugOverlay> m_debug_overlay;
	std::unique_ptr<Client> m_client;
};