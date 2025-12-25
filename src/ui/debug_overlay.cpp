#include "debug_overlay.h"
#include "../ui/ui.h"
#include "../ui/elements/text.h"
#include "../utils/resource/resources.h"
#include "../core/logger.h"
#include "../ecs/core/ecs.h"
#include "../world/world.h"
#include "../world/generation/world_generator.h"
#include "elements/vertical_layout.h"
#include "elements/crosshair.h"

#include <sstream>
#include <iomanip>

DebugOverlay::DebugOverlay(UI* ui, Resources* resources, World* world)
	: m_ui(ui), m_resources(resources), m_world(world), m_ecs(m_world->getECS())
{
	createElements();

	m_ui->updateScreenSize(m_ui->getScreenWidth(), m_ui->getScreenHeight());
}

DebugOverlay::~DebugOverlay() = default;

void DebugOverlay::createElements() {
	Font* font = m_resources->getFont("arial_22");
	Shader* shader = m_resources->getShader("text_shader");
	const glm::vec3 base_color = glm::vec3(1.0f, 1.0f, 1.0f);

	auto panel = std::make_unique<UIVerticalLayout>();
	panel->setPosition(5, 5, Anchor::TOP_LEFT);
	panel->setId("debug_panel");

	auto fps_text = std::make_unique<UIText>("", 0, 0, base_color, font, shader);
	fps_text->setId("debug_fps");

	auto pos_text = std::make_unique<UIText>("", 0, 0, base_color, font, shader);
	pos_text->setId("debug_player_info");

	auto world_gen_text_chunk = std::make_unique<UIText>("", 0, 0, base_color, font, shader);
	world_gen_text_chunk->setId("world_generator_info_create_chunk");

	auto world_gen_mesh_chunk = std::make_unique<UIText>("", 0, 0, base_color, font, shader);
	world_gen_mesh_chunk->setId("world_generator_info_create_mesh");

	panel->addChild(std::move(fps_text));
	panel->addChild(std::move(pos_text));
	panel->addChild(std::move(world_gen_text_chunk));
	panel->addChild(std::move(world_gen_mesh_chunk));

	m_ui->addElement(std::move(panel));

	// test
	Shader* crosshair_shader = m_resources->getShader("crosshair");
	auto crosshair = std::make_unique<UICrosshair>();
	crosshair->setShader(crosshair_shader);
	m_ui->addElement(std::move(crosshair));
}

void DebugOverlay::show() {
	m_visible = true;
	if (UIElement* fps = m_ui->getElementById("debug_fps")) fps->setVisible(true);
	if (UIElement* info = m_ui->getElementById("debug_player_info")) info->setVisible(true);
	if (UIElement* create_chunk = m_ui->getElementById("world_generator_info_create_chunk")) create_chunk->setVisible(true);
}

void DebugOverlay::hide() {
	m_visible = false;
	if (UIElement* fps = m_ui->getElementById("debug_fps")) fps->setVisible(false);
	if (UIElement* info = m_ui->getElementById("debug_player_info")) info->setVisible(false);
	if (UIElement* create_chunk = m_ui->getElementById("world_generator_info_create_chunk")) create_chunk->setVisible(false);
}

void DebugOverlay::toggle() {
	if (m_visible) {
		hide();
	}
	else {
		show();
	}
}


void DebugOverlay::update(const float delta_time) {
	updateFPS();
	updatePlayerInfo();
	updateWorldGeneratorInfo();
	updateMeshGeneratorInfo();
}

void DebugOverlay::updateFPS() {
	int fps = m_ui->getFPS();
	UIText* fps_text = m_ui->getElementById<UIText>("debug_fps");

	fps_text->setString(std::format("FPS: {}", fps));
	if (fps < 30) fps_text->setColor({ 1.0f, 0.0f, 0.0f });
	else if (fps < 60) fps_text->setColor({ 1.0f, 1.0f, 0.0f });
	else fps_text->setColor({ 0.0f, 1.0f, 0.0f });
}

void DebugOverlay::setEntity(Entity entity) {
	m_player_entity = entity;
}

void DebugOverlay::updatePlayerInfo() {
	UIText* player_info_text = m_ui->getElementById<UIText>("debug_player_info");

	auto& player_camera = m_ecs->storage<PlayerCamera>().get(m_player_entity);
	auto& player_transform = m_ecs->storage<Transform>().get(m_player_entity);
	auto& player_collider = m_ecs->storage<Collider>().get(m_player_entity);

	std::string output_str = std::format(
		"Position: {:.2f} {:.2f} {:.2f} / Rotate: {:.2f} {:.2f}",
		player_transform.position.x,
		player_transform.position.y - player_collider.half_y,
		player_transform.position.z,
		player_camera.yaw,
		player_camera.pitch
	);

	player_info_text->setString(output_str);
}

void DebugOverlay::setColor(const glm::vec3& color) {
	if (UIText* fps = m_ui->getElementById<UIText>("debug_fps")) fps->setColor(color);
	if (UIText* pos = m_ui->getElementById<UIText>("debug_player_info")) pos->setColor(color);
	if (UIText* world_generator_info = m_ui->getElementById<UIText>("world_generator_info_create_chunk")) world_generator_info->setColor(color);
}

void DebugOverlay::setScale(float scale) {
	if (UIText* fps = m_ui->getElementById<UIText>("debug_fps")) fps->setScale(scale);
	if (UIText* pos = m_ui->getElementById<UIText>("debug_player_info")) pos->setScale(scale);
	if (UIText* world_generator_info = m_ui->getElementById<UIText>("world_generator_info_create_chunk")) world_generator_info->setScale(scale);
}

void DebugOverlay::updateWorldGeneratorInfo() {
	UIText* world_info_text = m_ui->getElementById<UIText>("world_generator_info_create_chunk");

	std::string output_str = std::format(
		"Chunk generation time: {:.2f} ms",
		m_world->getGenerator()->getChunkGenerationTime() * 1000
	);

	world_info_text->setString(output_str);
}

void DebugOverlay::updateMeshGeneratorInfo() {
	UIText* world_info_text = m_ui->getElementById<UIText>("world_generator_info_create_mesh");

	std::string output_str = std::format(
		"Chunk mesh generation time: {:.2f} ms",
		m_world->getMeshGenerationTime() * 1000
	);

	world_info_text->setString(output_str);
}