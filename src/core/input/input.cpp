#include "input.h"
#include "../logger.h"

Input::Input() {
    m_mouse_pos_x = 0.0f;
    m_mouse_pos_y = 0.0f;

    m_mouse_delta_x = 0.0f;
    m_mouse_delta_y = 0.0f;

    for(int i = 0; i < KEYS_COUNT + MOUSE_BUTTONS; i++) {
        m_jkeys[i] = false;
        m_keys[i] = false;
    }

    m_first_move = true;
}
Input::~Input() = default;

void Input::press_button(const int key) {
    if (key < 0 || key > KEYS_COUNT) {
        LOG_WARN("Invalid key code: {0}", key);
        return;
    }
    m_jkeys[key] = true;
}
void Input::release_button(const int key) {
    if (key < 0 || key > KEYS_COUNT) {
        LOG_WARN("Invalid key code: {0}", key);
        return;
    }
    m_jkeys[key] = false;
}

void Input::mouse_press_button(const int key) {
    if (key < 0 || key > MOUSE_BUTTONS) {
        LOG_WARN("Invalid key code: {0}", key);
        return;
    }
    m_jkeys[KEYS_COUNT + key] = true;
}
void Input::mouse_release_button(const int key) {
    if (key < 0 || key > MOUSE_BUTTONS) {
        LOG_WARN("Invalid key code: {0}", key);
        return;
    }
    m_jkeys[KEYS_COUNT + key] = false;
}

void Input::mouse_move(double x_pos, double y_pos) {
    if(m_first_move) {
        m_mouse_pos_x = x_pos;
        m_mouse_pos_y = y_pos;

        m_first_move = false;
    }

    double delta_x = m_mouse_pos_x - x_pos;
    double delta_y = y_pos - m_mouse_pos_y;
    
    m_mouse_delta_x += -delta_x;
    m_mouse_delta_y += -delta_y;

    m_mouse_pos_x = x_pos;
    m_mouse_pos_y = y_pos;
}

void Input::update_input() {
    for(int i = 0; i < KEYS_COUNT + MOUSE_BUTTONS; i++) {
        m_keys[i] = m_jkeys[i];
    }

    m_mouse_delta_x = 0.0f;
    m_mouse_delta_y = 0.0f;
}

bool Input::pressed(const int key) const {
    if (key < 0 || key > KEYS_COUNT) {
        LOG_WARN("Invalid key code: {0}", key);
        return false;
    }
    return m_jkeys[key];
}
bool Input::jpressed(const int key) const {
    if(key < 0 || key > KEYS_COUNT) {
        LOG_WARN("Invalid key code: {0}", key);
        return false;
    }
    if(m_jkeys[key] && !m_keys[key]) return true;
    else return false;
}

bool Input::clicked(const int key) const {
    if(key < 0 || key > MOUSE_BUTTONS) {
        LOG_WARN("Invalid key code: {0}", key);
        return false;
    }
    return m_jkeys[KEYS_COUNT + key];
}
bool Input::jclicked(const int key) const {
    if(key < 0 || key > MOUSE_BUTTONS) {
        LOG_WARN("Invalid key code: {0}", key);
        return false;
    }
    if(m_jkeys[KEYS_COUNT + key] && !m_keys[KEYS_COUNT + key]) return true;
    else return false;
}

void Input::scroll(double x_offset, double y_offset) {

}