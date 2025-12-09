#include "input_handler.h"
#include "../game.h"
#include "input.h"
#include "../../world/chunk.h"
#include "../../world/block.h"

#include <GLFW/glfw3.h>

InputHandler::InputHandler(Game* game, Input* input, ECS* ecs) {
    m_player_entity = INVALID_ENTITY;

    m_input = input;
    m_ecs = ecs;
    m_game = game;
}
InputHandler::~InputHandler() {

}

bool current_render_debug_mode = false;

BlockID selected_block = BlockID::STONE;

void InputHandler::processing() {
    const float mouse_sensivty = 0.3f;

	double system_tick_time = m_game->getSystemInfo().update_tick_time;

	if (m_input->jpressed(GLFW_KEY_ESCAPE)) m_game->quit();

	auto& player_input = m_ecs->storage<PlayerInput>().get(m_player_entity);

	if (m_input->pressed(GLFW_KEY_W)) player_input.move_forward = 1.0f;
	else if (m_input->pressed(GLFW_KEY_S)) player_input.move_forward = -1.0f;
	else player_input.move_forward = 0.0f;

	if (m_input->pressed(GLFW_KEY_D)) player_input.move_right = 1.0f;
	else if (m_input->pressed(GLFW_KEY_A)) player_input.move_right = -1.0f;
	else player_input.move_right = 0.0f;

	if (m_input->pressed(GLFW_KEY_SPACE)) player_input.fly_up = true;
	else player_input.fly_up = false;

	if (m_input->pressed(GLFW_KEY_LEFT_SHIFT)) player_input.fly_down = true;
	else player_input.fly_down = false;

	player_input.mouse_delta_x = m_input->m_mouse_delta_x * mouse_sensivty;
	player_input.mouse_delta_y = m_input->m_mouse_delta_y * mouse_sensivty;

	if (m_input->jpressed(GLFW_KEY_M)) {
		auto& player_state = m_ecs->storage<PlayerState>().get(m_player_entity);

        if (player_state.mode == PlayerMode::CREATIVE) player_state.mode = PlayerMode::SURVIVAL;
        else if (player_state.mode == PlayerMode::SURVIVAL) player_state.mode = PlayerMode::SPECTATOR;
        else player_state.mode = PlayerMode::CREATIVE;
	}

	if (m_input->pressed(GLFW_KEY_SPACE)) {
		player_input.jump = true;
	}
	else player_input.jump = false;

	if (m_input->pressed(GLFW_KEY_LEFT_SHIFT)) {
		player_input.sneak = true;
	}
	else player_input.sneak = false;

    if (m_input->jpressed(GLFW_KEY_O)) {
        player_input.fly_speedup *= 2.0f;
    }

    if (m_input->jpressed(GLFW_KEY_P)) {
		player_input.fly_speedup *= 0.5f;
	}

    m_player_camera_system.update(*m_ecs, system_tick_time);
}

/*void InputHandler::processing() {
    Input* input = m_game_context->input;
    double system_tick_time = m_game_context->game->getSystemInfo().update_tick_time;
    ECS* ecs = m_game_context->world->getECS();

    if(input->jpressed(GLFW_KEY_ESCAPE)) m_game_context->game->quit();
    
    auto& player_input = ecs->storage<PlayerInput>().get(m_player_entity);
    
    if (input->pressed(GLFW_KEY_W)) player_input.move_forward = 1.0f;
    else if (input->pressed(GLFW_KEY_S)) player_input.move_forward = -1.0f;
    else player_input.move_forward = 0.0f;

	if (input->pressed(GLFW_KEY_D)) player_input.move_right = 1.0f;
	else if (input->pressed(GLFW_KEY_A)) player_input.move_right = -1.0f;
	else player_input.move_right = 0.0f;

    player_input.mouse_delta_x = input->m_mouse_delta_x;
    player_input.mouse_delta_y = input->m_mouse_delta_y;
    
	if (input->jpressed(GLFW_KEY_M)) {
        auto& player_state = ecs->storage<PlayerState>().get(m_player_entity);
	    
        if (player_state.mode == PlayerMode::CREATIVE) player_state.mode = PlayerMode::SURVIVAL;
        else player_state.mode = PlayerMode::CREATIVE;
    }

    if (input->pressed(GLFW_KEY_SPACE)) {
        player_input.jump = true;
    }
    else player_input.jump = false;
    
    if (input->pressed(GLFW_KEY_LEFT_SHIFT)) {
        player_input.sneak = true;
    } player_input.sneak = false;

    m_player_camera_system.update(*ecs, (float)system_tick_time);
    m_player_movement_system.update(*ecs, (float)system_tick_time);
    /*
    if (input->jclicked(GLFW_MOUSE_BUTTON_1)) {
        const float max_dist = 10.f;
        const float step = 0.01f;

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

    if (input->jclicked(GLFW_MOUSE_BUTTON_2)) {
        const float max_dist = 10.f;
        const float step = 0.01f;

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
                curr_dist -= step;
                ray_pos = start_pos + direction * curr_dist;
                pos_x = (int)floor(ray_pos.x);
                pos_y = (int)floor(ray_pos.y);
                pos_z = (int)floor(ray_pos.z);

                m_game_context->world->setBlock(pos_x, pos_y, pos_z, selected_block);
                break;
            }

            curr_dist += step;
        }
    }

    if (input->jclicked(GLFW_MOUSE_BUTTON_3)) {
        const float max_dist = 10.f;
        const float step = 0.01f;

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
                selected_block = block->getBlockID();
                break;
            }

            curr_dist += step;
        }
        */

void InputHandler::setPlayerEntity(Entity entity) {
    m_player_entity = entity;
}