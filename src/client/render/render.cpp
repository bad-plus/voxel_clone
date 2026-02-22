#include "render.h"
#include "../core/game.h"
#include "graphics/texture_atlas.h"
#include "graphics/shader.h"
#include <core/world/chunk/chunk.h>
#include <core/logger.hpp>
#include "../world/world.h"
#include <core/ecs/core/ecs.h>
#include "../core/window/window.h"
#include "../utils/resource/resources.h"
#include "camera.hpp"
#include "graphics/text.h"
#include <utility>
#include <vector>
#include <cmath>
#include <algorithm>
#include <core/ecs/components/transform.h>
#include <core/ecs/components/player_camera.h>
#include "../ui/ui.h"
#include <core/constants.h>
#include "../core/client.h"
#include "graphics/render_params.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace Constants;

Render::Render(Window* window, Client* client, Resources* resources, UI* ui) {
	m_window = window;
	m_resources = resources;
	m_ui = ui;
	m_render_params = RenderParams::getInstance();

	initRender();

	m_debug_render_mode = false;
	m_render_dist = 15;

	m_client = client;
}

Render::~Render() = default;

void Render::initRender() const {
	m_render_params->setup();
	m_render_params->depthTest(true);
}

void Render::render() {
	GLFWwindow* window = m_window->getGLFWwindow();

	glfwGetFramebufferSize(window, &m_render_width, &m_render_height);
	glViewport(0, 0, m_render_width, m_render_height);

	glClearColor(0.25f, 0.25f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_debug_render_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_render_params->blend(false);
	m_render_params->depthTest(true);
	renderWorld(m_client->getWorld(), m_render_dist);

	m_render_params->blend(true);
	m_render_params->depthTest(false);
	m_ui->draw();

	glfwSwapBuffers(window);
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
	
	glm::vec3 sun_color = glm::vec3(1.0f, 1.0f, 1.0f);

	Shader* world_block_shader = m_resources->getShader("block_shader");
	if (world_block_shader == nullptr) {
		LOG_WARN("BLOCK SHADER NOT LOADED! {0}", (void*)world_block_shader);
		return;
	}
	world_block_shader->use();
	world_block_shader->uniformVec3("sunColor", sun_color.r, sun_color.g, sun_color.b);
	TextureAtlas* atlas = m_resources->getTextureAtlas();

	world_block_shader->use();
	
	glActiveTexture(GL_TEXTURE0);
	atlas->bind();
	world_block_shader->uniformi1("ourTexture1", 0);

	Entity player_entity = m_client->getPlayerEntity();

	glm::mat4 view(1.0f);
	view = getCameraViewMatrix(ecs, player_entity);

	const auto& player_transform = ecs->storage<Transform>().get(player_entity);
	const auto& player_camera = ecs->storage<Camera>().get(player_entity);

	glm::mat4 projection(1.0f);
	projection = glm::perspective(player_camera.zoom, (float)m_render_width / (float)m_render_height, 0.1f, 1000.0f);

	int chunk_offset_x = ((player_transform.position.x) / CHUNK_SIZE_X);
	int chunk_offset_z = ((player_transform.position.z) / CHUNK_SIZE_Z);

	auto chunks_around_entity = genCircleReady(chunk_offset_x, chunk_offset_z, render_dist);

	glm::mat4 model;

	// Draw opaque
	m_render_params->depthTest(true);
	m_render_params->depthMask(true);
	m_render_params->blend(false);

	for (const auto& [x, z] : chunks_around_entity) {
		model = glm::translate(
			glm::mat4(1.0f),
			glm::vec3((x * (float)CHUNK_SIZE_X), 0.0f, (z * (float)CHUNK_SIZE_Z)));

		glm::mat4 mat = projection * view * model;
		world_block_shader->uniformmat4fv("transform", mat);

		ClientChunk* chunk = world->getChunk(x, z, true);
		if (chunk != nullptr) {
			chunk->drawOpaque();
		}
	}

	// Draw cutout

	m_render_params->depthTest(true);
	m_render_params->depthMask(true);
	m_render_params->blend(false);

	for (const auto& [x, z] : chunks_around_entity) {
		model = glm::translate(
			glm::mat4(1.0f),
			glm::vec3((x * (float)CHUNK_SIZE_X), 0.0f, (z * (float)CHUNK_SIZE_Z)));

		glm::mat4 mat = projection * view * model;
		world_block_shader->uniformmat4fv("transform", mat);

		ClientChunk* chunk = world->getChunk(x, z, true);
		if (chunk != nullptr) {
			chunk->drawCutout();
		}
	}

	// Draw transparent

	m_render_params->blend(true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_render_params->depthMask(false);

	for (auto it = chunks_around_entity.rbegin(); it != chunks_around_entity.rend(); ++it) {
		const auto& [x, z] = *it;

		model = glm::translate(
			glm::mat4(1.0f),
			glm::vec3((x * (float)CHUNK_SIZE_X), 0.0f, (z * (float)CHUNK_SIZE_Z)));

		glm::mat4 mat = projection * view * model;
		world_block_shader->uniformmat4fv("transform", mat);

		ClientChunk* chunk = world->getChunk(x, z, true);
		if (chunk != nullptr) {
			chunk->drawTransparent();
		}
	}

	m_render_params->depthMask(true);
}