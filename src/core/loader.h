#pragma once

struct GameContext;

class Loader {
public:
    Loader(GameContext* context);
    ~Loader();

    void loadResources();
    void loadShaders();
    void loadAtlas();
private:
    GameContext* m_game_context;
};