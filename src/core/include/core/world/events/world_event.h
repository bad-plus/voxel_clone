#pragma once

template<typename T> class WorldEventManager;

template<typename T>
struct WorldEvent {
public:
    virtual ~WorldEvent() = default;
    virtual void apply(T& world, WorldEventManager<T>& manager) = 0;
};