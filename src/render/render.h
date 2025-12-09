#pragma once

#include "../ecs/core/entity.h"

struct World;
struct Window;
struct ECS;
struct Resources;

class Render {
public:
    Render(Window* window, ECS* ecs, Resources* recources);
    ~Render();

    void render();

    void setWorld(World* world);
    void setPlayerEntity(Entity entity);

    void setDebugRenderMode(bool mode);

    void renderWorld(World* world, int render_dist = 5);
private:
    Window* m_window;
    ECS* m_ecs;
    Resources* m_resources;

    void initRender();

    bool m_debug_render_mode;

    int m_render_width;
    int m_render_height;

    World* m_world;
    Entity m_player_entity;


    int m_render_dist;
};