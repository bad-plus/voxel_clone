#include "game.h"
#include "window/window.h"
#include "logger.h"
#include "../world/generation/world_generator.h"
#include "../ecs/core/ecs.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <thread>

Game::Game() {
	m_quit = false;

	initGLFW();
	initLogger();

	m_input = new Input();
	updateGameContext();

	m_window = new Window("Game test", 1700, 760, &m_game_context);
	updateGameContext();

	m_resources = new Resources();
	updateGameContext();

	m_loader = new Loader(&m_game_context);
	updateGameContext();

	m_render = new Render(&m_game_context);
	updateGameContext();

	m_loader->loadResources();

	srand((time(NULL)));
	int seed = 1 + rand();
	m_world_generator = new WorldGenerator(seed);
	LOG_INFO("World seed: {0}", seed);

	m_world = new World(&m_game_context, m_world_generator);
	m_render->setWorld(m_world);

	Entity player_entity = m_world->CreatePlayer();
	m_render->setPlayerEntity(player_entity);
	updateGameContext();

	m_input_handler = new InputHandler(&m_game_context);
	m_input_handler->setPlayerEntity(player_entity);

	std::thread world_generation_thread(&Game::worldGenerationThread, this);
	world_generation_thread.detach();

	std::thread world_updater_thread(&Game::worldUpdaterThread, this);
	world_updater_thread.detach();

	std::thread movement_updater_thread(&Game::movementUpdaterThread, this);
	movement_updater_thread.detach();

	updateGameContext();

	m_window->setCursorEnabled(false);
	startMainLoop();
}

void Game::movementUpdaterThread() {
	const int tickrate = 60;
	const double dt = 1.0 / tickrate;

	double accumulator = 0.0;
	double last_time = glfwGetTime();

	while (!m_quit) {
		double current_time = glfwGetTime();
		double frame_time = current_time - last_time;
		last_time = current_time;

		if (frame_time > 0.25) {
			frame_time = dt;
		}

		accumulator += frame_time;

		while (accumulator >= dt) {
			m_world->tick_movement();
			accumulator -= dt;
		}

		double time_until_next_tick = dt - accumulator;
		if (time_until_next_tick > 0.002) {
			int sleep_ms = static_cast<int>((time_until_next_tick - 0.001) * 1000);
			if (sleep_ms > 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
			}
		}
		else {
			std::this_thread::yield();
		}
	}
}

void Game::worldGenerationThread() {
    while (!m_quit) {
        m_world->processUpdateMeshQueue();
        m_world->processGenerationQueue();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void Game::worldUpdaterThread() {
	const int tickrate = 24;
	const double dt = 1.0 / tickrate;

	double accumulator = 0.0;
	double last_time = glfwGetTime();

	while (!m_quit) { 
		double current_time = glfwGetTime();
		double frame_time = current_time - last_time;
		last_time = current_time;

		if (frame_time > 0.25) {
			frame_time = dt;
		}

		accumulator += frame_time;

		while (accumulator >= dt) {
			m_world->tick();
			accumulator -= dt;
		}

		double time_until_next_tick = dt - accumulator;
		if (time_until_next_tick > 0.002) {
			int sleep_ms = static_cast<int>((time_until_next_tick - 0.001) * 1000);
			if (sleep_ms > 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
			}
		}
		else {
			std::this_thread::yield();
		}
	}
}

void Game::updateGameContext() {
    m_game_context.game = this;
    m_game_context.input = m_input;
    m_game_context.window = m_window;
    m_game_context.render = m_render;
    m_game_context.camera = m_camera;
    m_game_context.resources = m_resources;
    m_game_context.loader = m_loader;
    m_game_context.world = m_world;
}

void Game::quit() {
    m_quit = true;
    m_window->quit();
}

Game::~Game() {
    LOG_INFO("Closing application...");
    m_quit = true;

    delete m_window;
    delete m_input;
    delete m_camera;
    delete m_render;
    delete m_input_handler;
    delete m_resources;
    delete m_loader;
    delete m_world;
    delete m_world_generator;

    glfwTerminate();
}

void Game::startMainLoop() {
    while (!m_quit) {
        double start_game_tick_time = glfwGetTime();

        m_window->eventProcessing();
        m_input_handler->processing();

        double start_render_time = glfwGetTime();
        m_render->render();
        double end_render_time = glfwGetTime();

        m_input->update_input();

        double end_game_tick_time = glfwGetTime();

        m_game_system_info.update_tick_time = end_game_tick_time - start_game_tick_time;
        m_game_system_info.render_time = end_render_time - start_render_time;
        std::string title_str;
        title_str += "Update time: ";
        title_str += std::to_string(m_game_system_info.update_tick_time * 1000.0f);
        title_str += " ms | ";
        title_str += "Render time: ";
        title_str += std::to_string(m_game_system_info.render_time * 1000.0f);
        title_str += " ms";
        glfwSetWindowTitle(m_window->getGLFWwindow(), title_str.c_str());
    }
}

GameSystemInfo Game::getSystemInfo() {
    return m_game_system_info;
}

void Game::initGLFW() {
    LOG_INFO("Starting application...");
    if (!glfwInit()) {
        LOG_ERROR("GLFW not initialized");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}