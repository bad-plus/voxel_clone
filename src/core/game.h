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
struct World;

struct GameContext {
	Game* game;
	Window* window;
	Input* input;
	Render* render;
	Camera* camera;
	Resources* resources;
	Loader* loader;
	World* world;
};

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
	void updateGameContext();
	void worldGenerationThread();
	void worldUpdaterThread();
	void movementUpdaterThread();

	Window* m_window;
	bool m_quit;
	Input* m_input;
	Render* m_render;
	Camera* m_camera;
	InputHandler* m_input_handler;
	Resources* m_resources;
	Loader* m_loader;
	World* m_world;
	WorldGenerator* m_world_generator;
	GameContext m_game_context;
};