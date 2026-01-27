#include <core/network/enet_context.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <enet/enet.h>
#include <stdexcept>

ENetContext& ENetContext::instance() {
    static ENetContext ctx;
    return ctx;
}

ENetContext::ENetContext() {
    if (enet_initialize() != 0) {
        throw std::runtime_error("Failed to initialize ENet library.");
    }
}

ENetContext::~ENetContext() {
    enet_deinitialize();
}