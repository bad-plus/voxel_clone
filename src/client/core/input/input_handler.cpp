#include "input_handler.h"
#include "../game.h"
#include "input.h"
#include <core/world/chunk/chunk.h>
#include <core/world/block/block.h>
#include "../../ui/ui.h"
#include "../../render/camera.hpp"
#include <memory>
#include "../../world/world/world_event_list.h"
#include "../client.h"

#include <core/constants.h>

#include <GLFW/glfw3.h>

InputHandler::InputHandler(Game* game, Input* input, UI* ui, Client* client) {
	m_input = input;
	m_game = game;
	m_ui = ui;
	m_client = client;

    m_window_width = 0;
    m_window_height = 0;
}
InputHandler::~InputHandler() = default;

bool current_render_debug_mode = false;

BlockID selected_block = BlockID::BLUE;

void InputHandler::processing() {
	auto world = m_client->getWorld();
	auto ecs = world->getECS();
	auto player_entity = m_client->getPlayerEntity();

	const float mouse_sensivty = 0.3f;

	Time system_tick_time = m_game->getSystemInfo().update_tick_time;

	if (m_input->jpressed(GLFW_KEY_ESCAPE)) m_game->quit();

	int new_window_width;
	int new_window_height;
	m_input->getWindowSize(&new_window_width, &new_window_height);

	if (m_window_width != new_window_width || m_window_height != new_window_height) {
		m_window_width = new_window_width;
		m_window_height = new_window_height;
		m_ui->updateScreenSize(m_window_width, m_window_height);
	}

	auto& player_input = ecs->storage<PlayerInput>().get(player_entity);

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
	player_input.mouse_delta_y = -m_input->m_mouse_delta_y * mouse_sensivty;

	if (m_input->jpressed(GLFW_KEY_M)) {
		auto& player_state = ecs->storage<PlayerState>().get(player_entity);

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

	if (m_input->pressed(GLFW_KEY_O)) {
		float new_speed = player_input.fly_speedup + (player_input.fly_speedup * 0.05f);
		
		if (
			new_speed < Constants::MAX_SPECTATOR_PLAYER_SPEED_MULTIPLIER &&
			new_speed > Constants::MIN_SPECTATOR_PLAYER_SPEED_MULTIPLIER) {

			player_input.fly_speedup = new_speed;
		}
	}

	if (m_input->pressed(GLFW_KEY_P)) {
		float new_speed = player_input.fly_speedup - (player_input.fly_speedup * 0.05f);

		if (
			new_speed < Constants::MAX_SPECTATOR_PLAYER_SPEED_MULTIPLIER &&
			new_speed > Constants::MIN_SPECTATOR_PLAYER_SPEED_MULTIPLIER) {

			player_input.fly_speedup = new_speed;
		}
	}

	auto& player_camera = ecs->storage<Camera>().get(player_entity);

	if (m_input->clicked(GLFW_MOUSE_BUTTON_1)) {

		const float max_dist = 10.f;
		const float step = 0.01f;

		float curr_dist = 0.0f;

		glm::vec3 start_pos = player_camera.position;
		glm::vec3 direction = getCameraFront(ecs, player_entity);

		while (curr_dist < max_dist) {
			glm::vec3 ray_pos = start_pos + direction * curr_dist;

			int pos_x = (int)floor(ray_pos.x);
			int pos_y = (int)floor(ray_pos.y);
			int pos_z = (int)floor(ray_pos.z);

			Block* block = world->getBlock(pos_x, pos_y, pos_z);

			if (block != nullptr && block->getBlockID() != BlockID::EMPTY) {
				world->addEvent(
					std::make_unique<BreakBlockEvent>(glm::ivec3(pos_x, pos_y, pos_z), player_entity)
				, true);
				break;
			}

			curr_dist += step;
		}
	}

	if (m_input->clicked(GLFW_MOUSE_BUTTON_2)) {
		const float max_dist = 10.f;
		const float step = 0.01f;

		float curr_dist = 0.0f;

		glm::vec3 start_pos = player_camera.position;
		glm::vec3 direction = getCameraFront(ecs, player_entity);

		while (curr_dist < max_dist) {
			glm::vec3 ray_pos = start_pos + direction * curr_dist;

			int pos_x = (int)floor(ray_pos.x);
			int pos_y = (int)floor(ray_pos.y);
			int pos_z = (int)floor(ray_pos.z);

			Block* block = world->getBlock(pos_x, pos_y, pos_z);

			if (block != nullptr && block->getBlockID() != BlockID::EMPTY) {
				curr_dist -= step;
				ray_pos = start_pos + direction * curr_dist;
				pos_x = (int)floor(ray_pos.x);
				pos_y = (int)floor(ray_pos.y);
				pos_z = (int)floor(ray_pos.z);

				world->addEvent(
					std::make_unique<SetupBlockEvent>(glm::ivec3(pos_x, pos_y, pos_z), player_entity, selected_block)
				, true);
				break;
			}

			curr_dist += step;
		}
	}

	if (m_input->clicked(GLFW_MOUSE_BUTTON_3)) {
		const float max_dist = 10.f;
		const float step = 0.01f;

		float curr_dist = 0.0f;

		glm::vec3 start_pos = player_camera.position;
		glm::vec3 direction = getCameraFront(ecs, player_entity);

		while (curr_dist < max_dist) {
			glm::vec3 ray_pos = start_pos + direction * curr_dist;

			int pos_x = (int)floor(ray_pos.x);
			int pos_y = (int)floor(ray_pos.y);
			int pos_z = (int)floor(ray_pos.z);

			Block* block = world->getBlock(pos_x, pos_y, pos_z);

			if (block != nullptr && block->getBlockID() != BlockID::EMPTY) {
				selected_block = block->getBlockID();
				break; 
			}

			curr_dist += step;
		}

	}
}