#pragma once

#define KEYS_COUNT              512
#define MOUSE_BUTTONS           16

class Input {
public:
    Input();
    ~Input();

    void press_button(int key);
    void release_button(int key);

    void mouse_press_button(int key);
    void mouse_release_button(int key);

    void mouse_move(double x_pos, double y_pos);

    void update_input();

    bool pressed(int key);
    bool jpressed(int key);

    bool clicked(int key);
    bool jclicked(int key);

    double m_mouse_pos_x;
    double m_mouse_pos_y;

    double m_mouse_delta_x;
    double m_mouse_delta_y;
private:
    bool m_first_move;

    bool m_jkeys[KEYS_COUNT + MOUSE_BUTTONS];
    bool m_keys[KEYS_COUNT + MOUSE_BUTTONS]; // KEYBOARD KEYS .. , MOUSE KEYS ..
};