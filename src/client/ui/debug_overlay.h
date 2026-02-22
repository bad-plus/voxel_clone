#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <core/time.hpp>

#include <core/ecs/core/entity.h>

class UI;
class UIText;
struct Resources;
struct Client;
struct World;

class DebugOverlay {
public:
	DebugOverlay(UI* ui, Resources* resources, Client* client);
	~DebugOverlay();

	void show();
	void hide();
	void toggle();

	void update(const Time& delta_time);
	
	void setColor(const glm::vec3& color);
	void setScale(float scale);
private:
	void createElements();
	void updateFPS();
	void updatePlayerInfo();
	void updateWorldGeneratorInfo();
	void updateMeshGeneratorInfo();

	UI* m_ui;
	Resources* m_resources;
	Client* m_client;

	bool m_visible = true;
};