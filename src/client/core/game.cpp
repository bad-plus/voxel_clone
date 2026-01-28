#include "game.h"
#include <core/logger.hpp>

#include "window/window.h"
#include <core/ecs/core/ecs.h>
#include "../utils/resource/resources.h"
#include "../utils/resource/loader.h"
#include "../world/world.h"
#include "../world/generation/world_generator.h"
#include "../render/render.h"
#include "input/input.h"
#include "input/input_handler.h"
#include "../ui/ui.h"
#include "../ui/debug_overlay.h"
#include <core/time.hpp>
#include "client.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <thread>
#include <random>
#include <memory>

constexpr int WORLD_MOVEMENT_TICKRATE = 90;
constexpr int WORLD_UPDATER_TICKRATE = 24;

Game::Game() {
	m_quit.store(false);

	initGLFW();
	initSystems();

	m_window->setWindowSize(1280, 720);
	m_window->setCursorEnabled(false);
	m_debug_overlay->show();

	m_threads.emplace_back(&Game::worldUpdaterThread, this);
	m_threads.emplace_back(&Game::movementUpdaterThread, this);
}

void Game::initSystems() {
	m_input = std::make_unique<Input>();

	m_window = std::make_unique<Window>("Voxel clone", 800, 600, m_input.get());

	m_resources = std::make_unique<Resources>();

	m_loader = std::make_unique<Loader>(m_resources.get());
	m_loader->loadResources();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1, INT_MAX);
	int seed = dis(gen);

#ifdef _DEBUG
	seed = 666;
#endif

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

	m_client = std::make_unique<Client>();
}

void Game::movementUpdaterThread() {
	const auto dt = Time::fromNS(1'000'000'000 / WORLD_MOVEMENT_TICKRATE);

	Time accumulator;
	auto last_time = Time::now();

	while (!m_quit) {
		auto current_time = Time::now();
		auto frame_time = current_time - last_time;
		last_time = current_time;

		if (frame_time > Time::fromMS(250)) {
			frame_time = dt;
		}

		accumulator += frame_time;

		while (accumulator >= dt) {
			m_world->tick_movement();
			accumulator -= dt;
		}

		auto time_until_next_tick = dt - accumulator;
		if (time_until_next_tick > Time::fromMS(2)) {
			long long sleep_ms = time_until_next_tick.getMS<long long>() - 1;
			if (sleep_ms > 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
			}
		}
		else {
			std::this_thread::yield();
		}
	}
}

void Game::worldUpdaterThread() {
	const auto dt = Time::fromNS(1'000'000'000 / WORLD_UPDATER_TICKRATE);

	auto accumulator = Time();
	auto last_time = Time::now();

	while (!m_quit) {
		auto current_time = Time::now();
		auto frame_time = current_time - last_time;
		last_time = current_time;

		if (frame_time > Time::fromMS(250)) {
			frame_time = dt;
		}

		accumulator += frame_time;

		while (accumulator >= dt) {
			m_world->tick();
			accumulator -= dt;
		}

		auto time_until_next_tick = dt - accumulator;
		if (time_until_next_tick > Time::fromMS(2)) {
			long long sleep_ms = time_until_next_tick.getMS<long long>() - 1;
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
	if (m_world) m_world->shutdown();
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
	m_client->connect("127.0.0.1");

	while (!m_quit) {
		auto start_game_tick_time = Time::now();

		m_window->eventProcessing();
		m_input_handler->processing();

		auto start_render_time = Time::now();
		m_render->render();
		auto end_render_time = Time::now();

		m_input->update_input();

		auto end_game_tick_time = Time::now();

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