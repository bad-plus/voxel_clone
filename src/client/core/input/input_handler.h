#pragma once
#include <core/ecs/core/ecs.h>

struct ECS;
struct Input;
struct Game;
struct World;
struct UI;

class InputHandler {
public:
    InputHandler(Game* game, Input* input, ECS* ecs, UI* ui, World* world);
    ~InputHandler();

    void processing();
    void setPlayerEntity(Entity entity);
private:
    Game* m_game;
    ECS* m_ecs;
    Input* m_input;
    UI* m_ui;
    Entity m_player_entity;
    World* m_world;

    int m_window_width;
    int m_window_height;
};