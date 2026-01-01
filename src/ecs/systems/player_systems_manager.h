#pragma once
#include "../core/ecs.h"
#include "player_collider_system.h"
#include "survival_movement_system.h"
#include "creative_movement_system.h"
#include "spectator_movement_system.h"

class PlayerSystemsManager {
public:
	void update(ECS& ecs) {
		m_collider_system.update(ecs);
		m_survival_movement.update(ecs);
		m_creative_movement.update(ecs);
		m_spectator_movement.update(ecs);
	}
private:
	PlayerColliderSystem m_collider_system;
	SurvivalMovementSystem m_survival_movement;
	CreativeMovementSystem m_creative_movement;
	SpectatorMovementSystem m_spectator_movement;
};