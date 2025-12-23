#pragma once
#include "../../ecs/core/ecs.h"
#include "../../ecs/systems/player_camera_system.h"
#include "../../ecs/systems/player_movement_system.h"

struct ECS;
struct Input;
struct Game;
struct World;
struct UI;

class InputHandler {
public:
    InputHandler(Game* game, Input* input, ECS* ecs, UI* ui);
    ~InputHandler();

    void processing();
    void setPlayerEntity(Entity entity);
private:
    Game* m_game;
    ECS* m_ecs;
    Input* m_input;
    UI* m_ui;
    Entity m_player_entity;

    CameraSystem m_player_camera_system;
    PlayerMovementSystem m_player_movement_system;

    int m_window_width;
    int m_window_height;
};