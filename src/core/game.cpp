#include "game.h"
#include "logger.h"

#include "window/window.h"
#include "../ecs/core/ecs.h"
#include "../utils/resource/resources.h"
#include "../utils/resource/loader.h"
#include "../world/world.h"
#include "../world/generation/world_generator.h"
#include "../render/render.h"
#include "input/input_handler.h"
#include "../ui/ui.h"
#include "../ui/debug_overlay.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <thread>
#include <random>
#include <memory>

constexpr int WORLD_MOVEMENT_TICKRATE = 60;
constexpr int WORLD_UPDATER_TICKRATE = 24;

Game::Game() {
	m_quit.store(false);
	initLogger();

	initGLFW();
	InitSystems();

	m_threads.emplace_back(&Game::worldGenerationThread, this);
	m_threads.emplace_back(&Game::worldUpdaterThread, this);
	m_threads.emplace_back(&Game::movementUpdaterThread, this);

	m_window->setWindowSize(1280, 720);
	m_window->setCursorEnabled(false);
	m_debug_overlay->show();
}

void Game::InitSystems() {
	m_input = std::make_unique<Input>();

	m_window = std::make_unique<Window>("Game test", 800, 600, m_input.get());

	m_resources = std::make_unique<Resources>();

	m_loader = std::make_unique<Loader>(m_resources.get());
	m_loader->loadResources();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1, INT_MAX);
	int seed = dis(gen);
	seed = 666;

	m_world_generator = std::make_unique<WorldGenerator>(seed);
	LOG_INFO("World seed: {0}", seed);

	m_world = std::make_unique<World>(m_world_generator.get());

	m_ui = std::make_unique<UI>(m_world->getECS(), m_resources.get());

	m_render = std::make_unique<Render>(m_window.get(), m_world->getECS(), m_resources.get(), m_ui.get());
	m_render->setWorld(m_world.get());

	Entity player_entity = m_world->CreatePlayer();
	m_render->setPlayerEntity(player_entity);

	m_input_handler = std::make_unique<InputHandler>(this, m_input.get(), m_world->getECS(), m_ui.get(), m_world.get());
	m_input_handler->setPlayerEntity(player_entity);

	m_debug_overlay = std::make_unique<DebugOverlay>(m_ui.get(), m_resources.get(), m_world.get());
	m_debug_overlay->setEntity(player_entity);
}

void Game::movementUpdaterThread() {
	const double dt = 1.0 / WORLD_MOVEMENT_TICKRATE;

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
        m_world->processGenerationQueue();
		m_world->processUpdateMeshQueue();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Game::worldUpdaterThread() {
	const double dt = 1.0 / WORLD_UPDATER_TICKRATE;

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

void Game::run() {
    while (!m_quit) {
        double start_game_tick_time = glfwGetTime();

        m_window->eventProcessing();
        m_input_handler->processing();

        double start_render_time = glfwGetTime();
        m_render->render();
        double end_render_time = glfwGetTime();

        m_input->update_input();

        double end_game_tick_time = glfwGetTime();

		m_debug_overlay->update(end_game_tick_time);

        m_game_system_info.update_tick_time = end_game_tick_time - start_game_tick_time;
        m_game_system_info.render_time = end_render_time - start_render_time;
    }
}

GameSystemInfo Game::getSystemInfo() const {
    return m_game_system_info;
}

void Game::initGLFW() const {
    LOG_INFO("Starting application...");
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}