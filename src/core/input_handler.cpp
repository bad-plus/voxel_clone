#include "input_handler.h"
#include "game.h"
#include "input.h"

#include <GLFW/glfw3.h>

InputHandler::InputHandler(GameContext* context) {
    m_game_context = context;
}
InputHandler::~InputHandler() {

}

bool current_render_debug_mode = false;

void InputHandler::processing() {
    Input* input = m_game_context->input;
    double system_tick_time = m_game_context->game->getSystemInfo().update_tick_time;

    if(input->jpressed(GLFW_KEY_ESCAPE)) m_game_context->game->quit();
    
    m_game_context->camera->processMouseMovement(input->m_mouse_delta_x, input->m_mouse_delta_y);

    if(input->pressed(GLFW_KEY_W)) m_game_context->camera->processKeyboard(FORWARD, system_tick_time);
    if(input->pressed(GLFW_KEY_S)) m_game_context->camera->processKeyboard(BACKWARD, system_tick_time);
    if(input->pressed(GLFW_KEY_A)) m_game_context->camera->processKeyboard(LEFT, system_tick_time);
    if(input->pressed(GLFW_KEY_D)) m_game_context->camera->processKeyboard(RIGHT, system_tick_time);

    if(input->jpressed(GLFW_KEY_SPACE)) {
        current_render_debug_mode = !current_render_debug_mode;
        m_game_context->render->setDebugRenderMode(current_render_debug_mode);
    }

    if (input->jpressed(GLFW_KEY_R)) {
        m_game_context->camera->m_position.x = rand();
        m_game_context->camera->m_position.z = rand();
    }
}