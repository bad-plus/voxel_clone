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
#include "../ui/ui.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Render::Render(Window* window, ECS* ecs, Resources* resources, UI* ui) {
	m_window = window;
	m_ecs = ecs;
	m_resources = resources;
	m_ui = ui;

	initRender();

	m_debug_render_mode = false;
	m_render_dist = 12;

	m_world = nullptr;
}

Render::~Render() = default;

void Render::initRender() const {
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
	glEnable(GL_DEPTH_TEST);
	renderWorld(m_world, m_render_dist);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	m_ui->draw();
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

	Shader* world_block_shader_opaque = m_resources->getShader("block_shader");
	if (world_block_shader_opaque == nullptr) {
		LOG_WARN("BLOCK SHADER NOT LOADED! {0}", (void*)world_block_shader_opaque);
		return;
	}

	Shader* world_block_shader_cutout = m_resources->getShader("block_shader");
	if (world_block_shader_cutout == nullptr) {
		LOG_WARN("BLOCK SHADER NOT LOADED! {0}", (void*)world_block_shader_cutout);
		return;
	}

	TextureAtlas* atlas = m_resources->getTextureAtlas();

	world_block_shader_opaque->use();

	glActiveTexture(GL_TEXTURE0);
	atlas->bind();
	world_block_shader_opaque->uniformi1("ourTexture1", 0);

	glm::mat4 view(1.0f);
	view = getCameraViewMatrix(ecs, m_player_entity);

	const auto& player_transform = ecs->storage<Transform>().get(m_player_entity);
	const auto& player_camera = ecs->storage<PlayerCamera>().get(m_player_entity);

	glm::mat4 projection(1.0f);
	projection = glm::perspective(player_camera.zoom, (float)m_render_width / (float)m_render_height, 0.1f, 1000.0f);

	int chunk_offset_x = (player_transform.position.x / CHUNK_SIZE_X);
	int chunk_offset_z = (player_transform.position.z / CHUNK_SIZE_Z);

	auto chunks_to_draw = genCircleReady(chunk_offset_x, chunk_offset_z, render_dist);

	// Draw opaque
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	for (const auto& [x, z] : chunks_to_draw) {
		glm::mat4 model = glm::translate(
			glm::mat4(1.0f),
			glm::vec3((x * (float)CHUNK_SIZE_X), 0.0f, (z * (float)CHUNK_SIZE_Z)));

		glm::mat4 mat = projection * view * model;
		world_block_shader_opaque->uniformmat4fv("transform", mat);

		Chunk* chunk = world->getChunk(x, z, true);
		if (chunk != nullptr) {
			chunk->drawOpaque();
		}
	}

	// Draw cutout

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	for (const auto& [x, z] : chunks_to_draw) {
		glm::mat4 model = glm::translate(
			glm::mat4(1.0f),
			glm::vec3((x * (float)CHUNK_SIZE_X), 0.0f, (z * (float)CHUNK_SIZE_Z)));

		glm::mat4 mat = projection * view * model;
		world_block_shader_opaque->uniformmat4fv("transform", mat);

		Chunk* chunk = world->getChunk(x, z, true);
		if (chunk != nullptr) {
			chunk->drawCutout();
		}
	}

	// Draw transparent
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	for (auto it = chunks_to_draw.rbegin(); it != chunks_to_draw.rend(); ++it) {
		const auto& [x, z] = *it;

		glm::mat4 model = glm::translate(
			glm::mat4(1.0f),
			glm::vec3((x * (float)CHUNK_SIZE_X), 0.0f, (z * (float)CHUNK_SIZE_Z)));

		glm::mat4 mat = projection * view * model;
		world_block_shader_opaque->uniformmat4fv("transform", mat);

		Chunk* chunk = world->getChunk(x, z, true);
		if (chunk != nullptr) {
			chunk->drawTransparent();
		}
	}

	glDepthMask(GL_TRUE);
}

void Render::setPlayerEntity(Entity entity) {
	m_player_entity = entity;
}