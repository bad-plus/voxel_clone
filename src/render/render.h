#pragma once

#include "../ecs/core/entity.h"

struct Camera;
struct GameContext;
struct World;

class Render {
public:
    Render(GameContext* game_context);
    ~Render();

    void render();

    void setWorld(World* world);
    void setPlayerEntity(Entity entity);

    void setDebugRenderMode(bool mode);

    void renderWorld(World* world, int render_dist = 5);
private:
    void initRender();

    bool m_debug_render_mode;

    int m_render_width;
    int m_render_height;

    GameContext* m_game_context;

    World* m_world;
    Entity m_player_entity;


    int m_render_dist;
};