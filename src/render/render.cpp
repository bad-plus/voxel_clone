#include "render.h"
#include "../core/game.h"
#include "graphics/texture_atlas.h"
#include "graphics/shader.h"
#include "../world/chunk.h"
#include "../core/logger.h"
#include "../world/world.h"
#include "../ecs/core/ecs.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Render::Render(GameContext* game_context) {
    m_game_context = game_context;
    initRender();

    m_debug_render_mode = false;
    m_render_dist = 15;

    m_world = nullptr;
}
Render::~Render() {

}

void Render::initRender() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
}

void Render::render() {
    GLFWwindow* window = m_game_context->window->getGLFWwindow();

    glfwGetFramebufferSize(window, &m_render_width, &m_render_height);
    glViewport(0, 0, m_render_width, m_render_height);

    glClearColor(0.25f, 0.25f, 0.75f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_debug_render_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    renderWorld(m_world, m_render_dist);

    glfwSwapBuffers(window);
}

void Render::setWorld(World* world) {
    m_world = world;
}

void Render::setDebugRenderMode(bool mode) {
    m_debug_render_mode = mode;
}

void Render::renderWorld(World* world, int render_dist) {
	if (world == nullptr) return;
	ECS* ecs = m_game_context->world->getECS();


	Shader* world_block_shader = m_game_context->resources->getShader("block_shader");
	if (world_block_shader == nullptr) {
		LOG_WARN("BLOCK SHADER NOT LOADED! {0}", (void*)world_block_shader);
		return;
	}

	TextureAtlas* atlas = m_game_context->resources->getTextureAtlas();

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

	for (int x = -render_dist; x < render_dist; x++) {
		for (int z = -render_dist; z < render_dist; z++) {
			float global_chunk_position_x = chunk_offset_x + x;
			float global_chunk_position_z = chunk_offset_z + z;

			glm::mat4 model = glm::translate(
				glm::mat4(1.0f),
				glm::vec3((float)(global_chunk_position_x * (float)CHUNK_SIZE_X),
					0.0f,
					(float)(global_chunk_position_z * (float)CHUNK_SIZE_Z)));

			glm::mat4 mat = projection * view * model;
			world_block_shader->uniformmat4fv("transform", mat);

			Chunk* chunk = world->getChunk(global_chunk_position_x, global_chunk_position_z, true);
			if (chunk != nullptr) {
				chunk->draw();
			}
		}
	}
}

void Render::setPlayerEntity(Entity entity) {
    m_player_entity = entity;
}