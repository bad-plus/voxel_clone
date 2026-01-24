#pragma once
#include <array>

constexpr unsigned int KEYS_COUNT =         512;
constexpr unsigned int MOUSE_BUTTONS =      16;

class Input {
public:
    Input();
    ~Input();

    void update_window_size(const int width, const int height);

    void press_button(const int key);
    void release_button(const int key);

    void mouse_press_button(const int key);
    void mouse_release_button(const int key);

    void mouse_move(double x_pos, double y_pos);

    void update_input();

    bool pressed(const int key) const;
    bool jpressed(const int key) const;

    bool clicked(const int key) const;
    bool jclicked(const int key) const;

    void scroll(double x_offset, double y_offset);

    double m_mouse_pos_x;
    double m_mouse_pos_y;

    double m_mouse_delta_x;
    double m_mouse_delta_y;

    void getWindowSize(int* width, int* height) const;
private:
    bool m_first_move;

    // KEYBOARD KEYS .. , MOUSE KEYS ..
    std::array<bool, KEYS_COUNT + MOUSE_BUTTONS> m_keys;
    std::array<bool, KEYS_COUNT + MOUSE_BUTTONS> m_jkeys;

    int m_window_width;
    int m_window_height;
};