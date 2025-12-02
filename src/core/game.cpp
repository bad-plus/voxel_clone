#include "game.h"
#include "window.h"
#include "logger.h"

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

    m_input_handler = new InputHandler(&m_game_context);

    m_window = new Window("Game test", 1280, 720, &m_game_context);
    updateGameContext();

    m_camera = new Camera({10.0f, 300.0f, 10.0f});
    updateGameContext();

    m_resources = new Resources();
    updateGameContext();

    m_loader = new Loader(&m_game_context);
    updateGameContext();

    m_render = new Render(&m_game_context);
    m_render->setCamera(m_camera);
    updateGameContext();

    m_loader->loadResources();
    
    m_world_generator = new WorldGeneator(777);

    m_world = new World(&m_game_context, m_world_generator);
    m_render->setWorld(m_world);

    std::thread world_generation_thread(&Game::worldGenerationThread, this);
    world_generation_thread.detach();

    m_window->setCursorEnabled(false);
    startMainLoop();
}

void Game::worldGenerationThread() {
    while(!m_quit) {
        m_world->processGenerationQueue();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
}

void Game::quit() {
    m_quit = true;
    m_window->quit();
}

Game::~Game() {
    LOG_INFO("Clossing application...");
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
    while(!m_quit) {
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
    if(!glfwInit()) {
        LOG_ERROR("GLFW not initialized");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}