#pragma once
#include <core/ecs/core/entity.h>

struct World;
struct Window;
struct ECS;
struct Resources;
struct UI;

class Render {
public:
    Render(Window* window, ECS* ecs, Resources* recources, UI* ui);
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
    UI* m_ui;

    void initRender() const;

    bool m_debug_render_mode;

    int m_render_width;
    int m_render_height;

    World* m_world;
    Entity m_player_entity;

    int m_render_dist;
};