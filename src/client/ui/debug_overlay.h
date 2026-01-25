#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <core/time.hpp>

#include "../ecs/core/entity.h"

class UI;
class UIText;
struct Resources;
struct ECS;
struct World;

class DebugOverlay {
public:
	DebugOverlay(UI* ui, Resources* resources, World* world);
	~DebugOverlay();

	void show();
	void hide();
	void toggle();

	void update(const Time& delta_time);
	
	void setColor(const glm::vec3& color);
	void setScale(float scale);

	void setEntity(Entity entity);
private:
	void createElements();
	void updateFPS();
	void updatePlayerInfo();
	void updateWorldGeneratorInfo();
	void updateMeshGeneratorInfo();

	UI* m_ui;
	Resources* m_resources;
	World* m_world;
	ECS* m_ecs;
	Entity m_player_entity;

	bool m_visible = true;
};