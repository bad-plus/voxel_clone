#pragma once
class ENetContext {
public:
    static ENetContext& instance();

    ENetContext(const ENetContext&) = delete;
    ENetContext& operator=(const ENetContext&) = delete;
private:
    ENetContext();
    ~ENetContext();
};