#pragma once
#include "window.h"
#include "input.h"
#include "../render/render.h"
#include "../render/camera.hpp"
#include "input_handler.h"
#include "resources.h"
#include "loader.h"
#include "../world/world.h"

class Game;

struct GameContext {
    Game* game;
    Window* window;
    Input* input;
    Render* render;
    Camera* camera;
    Resources* resources;
    Loader* loader;
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

    Window* m_window;
    bool m_quit;

    Input* m_input;
    Render* m_render;
    Camera* m_camera;
    InputHandler* m_input_handler;
    Resources* m_resources;
    Loader* m_loader;
    World* m_world;

    GameContext m_game_context;

};