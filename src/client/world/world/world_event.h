#pragma once
struct World;
struct WorldEventManager;

struct WorldEvent {
protected:
	bool m_dont_miss = true;
public:
	~WorldEvent() = default;
	virtual void apply(World& world, WorldEventManager& manager) = 0;

	bool isDontMiss() const { return m_dont_miss; }
};