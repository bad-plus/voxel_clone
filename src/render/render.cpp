#include "render.h"
#include "../core/game.h"
#include "graphics/texture_atlas.h"
#include "graphics/shader.h"
#include "../world/chunk.h"
#include "../core/logger.h"
#include "../world/world.h"
#include "../ecs/core/ecs.h"
#include "../core/window/window.h"
#include "../utils/resource/resources.h"
#include "camera.hpp"
#include "graphics/text.h"
#include <utility>
#include <vector>
#include <cmath>
#include <algorithm>
#include "../ecs/components/transform.h"
#include "../ecs/components/player_camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Render::Render(Window* window, ECS* ecs, Resources* resources) {
	m_window = window;
	m_ecs = ecs;
	m_resources = resources;

    initRender();

    m_debug_render_mode = false;
    m_render_dist = 20;

    m_world = nullptr;
}
Render::~Render() {

}

void Render::initRender() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Render::render() {
    GLFWwindow* window = m_window->getGLFWwindow();

    glfwGetFramebufferSize(window, &m_render_width, &m_render_height);
    glViewport(0, 0, m_render_width, m_render_height);

    glClearColor(0.25f, 0.25f, 0.75f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_debug_render_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisable(GL_BLEND);
    renderWorld(m_world, m_render_dist);
	
	renderDebug();
	glfwSwapBuffers(window);
}

void Render::setWorld(World* world) {
    m_world = world;
}

void Render::setDebugRenderMode(bool mode) {
    m_debug_render_mode = mode;
}

std::vector<std::pair<int, int>> genCircleReady(int cx, int cz, int radius) {
	std::vector<std::pair<int, int>> out;

	for (int x = -radius; x <= radius; x++) {
		for (int z = -radius; z <= radius; z++) {
			if (x * x + z * z <= radius * radius) {
				out.emplace_back(cx + x, cz + z);
			}
		}
	}

	std::sort(out.begin(), out.end(), [cx, cz](const auto& a, const auto& b) {
		int dist_a = (a.first - cx) * (a.first - cx) + (a.second - cz) * (a.second - cz);
		int dist_b = (b.first - cx) * (b.first - cx) + (b.second - cz) * (b.second - cz);
		return dist_a < dist_b;
		});

	return out;
}

void Render::renderWorld(World* world, int render_dist) {
	if (world == nullptr) return;
	ECS* ecs = world->getECS();


	Shader* world_block_shader = m_resources->getShader("block_shader");
	if (world_block_shader == nullptr) {
		LOG_WARN("BLOCK SHADER NOT LOADED! {0}", (void*)world_block_shader);
		return;
	}

	TextureAtlas* atlas = m_resources->getTextureAtlas();

	world_block_shader->use();

	glActiveTexture(GL_TEXTURE0);
	atlas->bind();
	world_block_shader->uniformi1("ourTexture1", 0);

	glm::mat4 view(1.0f);
	view = getCameraViewMatrix(ecs, m_player_entity);

	auto& player_input = ecs->storage<PlayerInput>().get(m_player_entity);
	auto& player_transform = ecs->storage<Transform>().get(m_player_entity);
	auto& player_camera = ecs->storage<PlayerCamera>().get(m_player_entity);

	glm::mat4 projection(1.0f);
	projection = glm::perspective(player_camera.zoom, (float)m_render_width / (float)m_render_height, 0.1f, 1000.0f);


	int chunk_offset_x = player_transform.position.x / CHUNK_SIZE_X;
	int chunk_offset_z = player_transform.position.z / CHUNK_SIZE_Z;

	auto go_draw = genCircleReady(chunk_offset_x, chunk_offset_z, render_dist);

	for (auto& [x, z] : go_draw) {
		glm::mat4 model = glm::translate(
			glm::mat4(1.0f),
			glm::vec3((float)(x * (float)CHUNK_SIZE_X),
				0.0f,
				(float)(z * (float)CHUNK_SIZE_Z)));

		glm::mat4 mat = projection * view * model;
		world_block_shader->uniformmat4fv("transform", mat);

		Chunk* chunk = world->getChunk(x, z, true);
		if (chunk != nullptr) {
			chunk->draw();
		}
	}
}

void Render::setPlayerEntity(Entity entity) {
    m_player_entity = entity;
}

void Render::renderDebug() {
	glEnable(GL_BLEND);
	Text debug_text(m_resources->getFont("arial_22"), "", 10, 690, 1.0f, { 1.0f, 1.0f, 1.0f }, m_render_width, m_render_height);
	debug_text.setShader(m_resources->getShader("text_shader"));

	ECS* ecs = m_world->getECS();

	auto& player_camera = ecs->storage<PlayerCamera>().get(m_player_entity);
	auto& player_transform = ecs->storage<Transform>().get(m_player_entity);

	std::string debug_str = "";
	debug_str += "Camera position: ";
	debug_str += std::to_string(player_transform.position.x) + " ";
	debug_str += std::to_string(player_transform.position.y) + " ";
	debug_str += std::to_string(player_transform.position.z);
	debug_str += "\n";
	debug_str += "Camera rotation: ";
	debug_str += "yaw: " + std::to_string(player_camera.yaw) + " ";
	debug_str += " | pitch: " + std::to_string(player_camera.pitch) + " ";
	debug_str += "\n";


	
	debug_text.setString(debug_str);
	debug_text.draw();
}