#include "debug_overlay.h"
#include "../ui/ui.h"
#include "../ui/elements/text.h"
#include "../utils/resource/resources.h"
#include "../core/logger.h"
#include "../ecs/core/ecs.h"
#include "../world/world.h"

#include <sstream>
#include <iomanip>

DebugOverlay::DebugOverlay(UI* ui, Resources* resources, World* world)
	: m_ui(ui), m_resources(resources), m_world(world), m_ecs(m_world->getECS())
{
	createElements();
}

DebugOverlay::~DebugOverlay() = default;

void DebugOverlay::createElements() {
	Font* font = m_resources->getFont("arial_22");
	Shader* shader = m_resources->getShader("text_shader");

	if (!font || !shader) {
		LOG_ERROR("Failed to load resources for debug overlay");
		return;
	}

	const glm::vec3 base_color = glm::vec3(1.0f, 1.0f, 1.0f);

	auto fps_text = std::make_unique<UIText>("", 10, 10, base_color, font, shader);
	fps_text->setId("debug_fps");
	m_ui->addElement(std::move(fps_text));

	auto pos_text = std::make_unique<UIText>("", 10, 40, base_color, font, shader);
	pos_text->setId("player_info");
	m_ui->addElement(std::move(pos_text));

	auto custom_text = std::make_unique<UIText>("", 10, 70, base_color, font, shader);
	custom_text->setId("debug_custom");
	m_ui->addElement(std::move(custom_text));
}

void DebugOverlay::show() {
	m_visible = true;
	if (UIElement* fps = m_ui->getElementById("debug_fps")) fps->setVisible(true);
	if (UIElement* pos = m_ui->getElementById("player_info")) pos->setVisible(true);
	if (UIElement* custom = m_ui->getElementById("debug_custom")) custom->setVisible(true);
}

void DebugOverlay::hide() {
	m_visible = false;
	if (UIElement* fps = m_ui->getElementById("debug_fps")) fps->setVisible(false);
	if (UIElement* pos = m_ui->getElementById("player_info")) pos->setVisible(false);
	if (UIElement* custom = m_ui->getElementById("debug_custom")) custom->setVisible(false);
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
	UIText* fps_text = m_ui->getElementById<UIText>("player_info");

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

	fps_text->setString(output_str);
}

void DebugOverlay::setColor(const glm::vec3& color) {
	if (UIText* fps = m_ui->getElementById<UIText>("debug_fps")) fps->setColor(color);
	if (UIText* pos = m_ui->getElementById<UIText>("player_info")) pos->setColor(color);
	if (UIText* custom = m_ui->getElementById<UIText>("debug_custom")) custom->setColor(color);
}

void DebugOverlay::setScale(float scale) {
	if (UIText* fps = m_ui->getElementById<UIText>("debug_fps")) fps->setScale(scale);
	if (UIText* pos = m_ui->getElementById<UIText>("player_info")) pos->setScale(scale);
	if (UIText* custom = m_ui->getElementById<UIText>("debug_custom")) custom->setScale(scale);
}