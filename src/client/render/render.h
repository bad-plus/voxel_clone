#pragma once
#include <core/ecs/core/entity.h>

struct World;
struct Window;
struct ECS;
struct Resources;
struct UI;
struct Client;

class Render {
public:
    Render(Window* window, Client* client, Resources* recources, UI* ui);
    ~Render();

    void render();

    void setDebugRenderMode(bool mode);

    void renderWorld(World* world, int render_dist = 5);
private:
    Window* m_window;
    Resources* m_resources;
    UI* m_ui;

    void initRender() const;

    bool m_debug_render_mode;

    int m_render_width;
    int m_render_height;

    Client* m_client;

    int m_render_dist;
};