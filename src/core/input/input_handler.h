#pragma once
#include "../../ecs/core/ecs.h"
#include "../../ecs/systems/player_camera_system.h"
#include "../../ecs/systems/player_movement_system.h"
#include "../../ecs/systems/world_collision_system.h"

struct GameContext;

class InputHandler {
public:
    InputHandler(GameContext* context);
    ~InputHandler();

    void processing();
    void setPlayerEntity(Entity entity);
private:
    GameContext* m_game_context;
    Entity m_player_entity;

    CameraSystem m_player_camera_system;
    PlayerMovementSystem m_player_movement_system;
    WorldCollisionSystem m_world_collision_system;
};