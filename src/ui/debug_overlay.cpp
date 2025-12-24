#include "debug_overlay.h"
#include "../ui/ui.h"
#include "../ui/elements/text.h"
#include "../utils/resource/resources.h"
#include "../core/logger.h"
#include "../ecs/core/ecs.h"
#include "../world/world.h"
#include "../world/generation/world_generator.h"
#include "elements/vertical_layout.h"

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
	pos_text->setId("player_info");

	auto world_gen_text = std::make_unique<UIText>("", 0, 0, base_color, font, shader);
	world_gen_text->setId("world_generator_info");

	panel->addChild(std::move(fps_text));
	panel->addChild(std::move(pos_text));
	panel->addChild(std::move(world_gen_text));

	m_ui->addElement(std::move(panel));
}

void DebugOverlay::show() {
	m_visible = true;
	if (UIElement* fps = m_ui->getElementById("debug_fps")) fps->setVisible(true);
	if (UIElement* pos = m_ui->getElementById("player_info")) pos->setVisible(true);
	if (UIElement* custom = m_ui->getElementById("world_generator_info")) custom->setVisible(true);
}

void DebugOverlay::hide() {
	m_visible = false;
	if (UIElement* fps = m_ui->getElementById("debug_fps")) fps->setVisible(false);
	if (UIElement* pos = m_ui->getElementById("player_info")) pos->setVisible(false);
	if (UIElement* world_generator_info = m_ui->getElementById("world_generator_info")) world_generator_info->setVisible(false);
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
	UIText* player_info_text = m_ui->getElementById<UIText>("player_info");

	auto& player_camera = m_ecs->storage<PlayerCamera>().get(m_player_entity);
	auto& player_transform = m_ecs->storage<Transform>().get(m_player_entity);
	auto& player_collider = m_ecs->storage<Collider>().get(m_player_entity);

	std::string output_str = std::format(
		"Position: {:.2f} {:.2f} {:.2f} / Rotate {:.2f} {:.2f}",
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
	if (UIText* pos = m_ui->getElementById<UIText>("player_info")) pos->setColor(color);
	if (UIText* world_generator_info = m_ui->getElementById<UIText>("world_generator_info")) world_generator_info->setColor(color);
}

void DebugOverlay::setScale(float scale) {
	if (UIText* fps = m_ui->getElementById<UIText>("debug_fps")) fps->setScale(scale);
	if (UIText* pos = m_ui->getElementById<UIText>("player_info")) pos->setScale(scale);
	if (UIText* world_generator_info = m_ui->getElementById<UIText>("world_generator_info")) world_generator_info->setScale(scale);
}

void DebugOverlay::updateWorldGeneratorInfo() {
	UIText* world_info_text = m_ui->getElementById<UIText>("world_generator_info");

	std::string output_str = std::format(
		"Chunk generation time: {:.2f} ms",
		m_world->getGenerator()->getChunkGenerationTime() * 1000
	);

	world_info_text->setString(output_str);
}