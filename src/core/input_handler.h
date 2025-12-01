#pragma once

struct GameContext;

class InputHandler {
public:
    InputHandler(GameContext* context);
    ~InputHandler();

    void processing();
private:
    GameContext* m_game_context;
};