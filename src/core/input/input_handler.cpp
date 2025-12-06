#include "input_handler.h"
#include "../game.h"
#include "input.h"
#include "../../world/chunk.h"
#include "../../world/block.h"

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

    if (input->jclicked(GLFW_MOUSE_BUTTON_1)) {
        const float max_dist = 10.f;
        const float step = 0.05f;

        float curr_dist = 0.0f;

        glm::vec3 start_pos = m_game_context->camera->m_position;
        glm::vec3 direction = glm::normalize(m_game_context->camera->m_front);

        while (curr_dist < max_dist) {
            glm::vec3 ray_pos = start_pos + direction * curr_dist;

            int pos_x = (int)floor(ray_pos.x);
            int pos_y = (int)floor(ray_pos.y);
            int pos_z = (int)floor(ray_pos.z);

            Block* block = m_game_context->world->getBlock(pos_x, pos_y, pos_z);

            if (block != nullptr && block->getBlockID() != BlockID::EMPTY) {
                m_game_context->world->setBlock(pos_x, pos_y, pos_z, BlockID::EMPTY);
                break;
            }

            curr_dist += step;
        }
    }

    if (input->pressed(GLFW_KEY_Q)) {
        m_game_context->camera->setSpeed(150);
    } else m_game_context->camera->setSpeed(10);
}