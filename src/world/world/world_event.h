#pragma once
struct World;
struct WorldEventManager;

struct WorldEvent {
public:
	~WorldEvent() = default;
	virtual void apply(World& world, WorldEventManager& manager) = 0;
};