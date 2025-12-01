#pragma once

struct Camera;
struct GameContext;
struct World;

class Render {
public:
    Render(GameContext* game_context);
    ~Render();

    void render();

    void setCamera(Camera* camera);
    void setWorld(World* world);

    void setDebugRenderMode(bool mode);

    void renderWorld(World* world, Camera* camera, int render_dist = 5);
private:
    void initRender();

    bool m_debug_render_mode;

    int m_render_width;
    int m_render_height;

    GameContext* m_game_context;

    Camera* m_camera;
    World* m_world;

    int m_render_dist;
};