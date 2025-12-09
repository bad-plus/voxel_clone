#include "game.h"
#include "window/window.h"
#include "logger.h"
#include "../world/generation/world_generator.h"
#include "../ecs/core/ecs.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <thread>
#include <random>

Game::Game() {
	m_quit.store(false);

	initGLFW();
	initLogger();

	m_input = std::make_unique<Input>();

	m_window = std::make_unique<Window>("Game test", 1280, 720, m_input.get());

	m_resources = std::make_unique<Resources>();

	m_loader = std::make_unique<Loader>(m_resources.get());

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1, INT_MAX);
	int seed = dis(gen);

	m_world_generator = std::make_unique<WorldGenerator>(seed);
	LOG_INFO("World seed: {0}", seed);

	m_world = std::make_unique<World>(m_world_generator.get());

	m_render = std::make_unique<Render>(m_window.get(), m_world->getECS(), m_resources.get());
	m_render->setWorld(m_world.get());

	m_loader->loadResources();

	Entity player_entity = m_world->CreatePlayer();
	m_render->setPlayerEntity(player_entity);

	m_input_handler = std::make_unique<InputHandler>(this, m_input.get(), m_world->getECS());
	m_input_handler->setPlayerEntity(player_entity);

	m_threads.emplace_back(&Game::worldGenerationThread, this);
	m_threads.emplace_back(&Game::worldUpdaterThread, this);
	m_threads.emplace_back(&Game::movementUpdaterThread, this);

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

void Game::quit() {
	m_quit.store(true);
    m_window->quit();
}

Game::~Game() {
    LOG_INFO("Closing application...");
	quit();

	for (auto& thread : m_threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}

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
		throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}