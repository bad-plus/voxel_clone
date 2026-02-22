#pragma once
#include <core/ecs/core/ecs.h>

struct Input;
struct Game;
struct Client;
struct UI;

class InputHandler {
public:
    InputHandler(Game* game, Input* input, UI* ui, Client* client);
    ~InputHandler();

    void processing();
private:
    Game* m_game;
    Input* m_input;
    UI* m_ui;
    Client* m_client;

    int m_window_width;
    int m_window_height;
};