#include "server.h"
#include "../enet_context.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <enet/enet.h>
#include <stdexcept>
#include "../../core/logger.h"

struct Server::Impl {
    ENetHost* host = nullptr;

    ~Impl() {
        if (host) {
            enet_host_destroy(host);
        }
    }

    void processEvent(ENetEvent& event) {
        LOG_INFO("[SERVER] process event: {0}", static_cast<int>(event.type));
    }
};

Server::Server() : m_enet_impl(std::make_unique<Impl>()), m_running(false) {
    ENetContext::instance();
}

Server::~Server() {
    LOG_INFO("[SERVER] Stopped.");
}

void Server::run(uint16_t port, size_t max_clients) {
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    m_enet_impl->host = enet_host_create(&address, max_clients, 2, 0, 0);

    if (!m_enet_impl->host) {
        throw std::runtime_error("Failed to create ENet server host.");
    }

    m_running = true;
    LOG_INFO("[SERVER] Started on port {0}, max clients: {1}", port, max_clients);

    while (m_running) {
        processNetwork();
    }
}

void Server::stop() {
    m_running = false;
}

void Server::processNetwork() {
    ENetEvent event;
    while (enet_host_service(m_enet_impl->host, &event, 0) > 0) {
        m_enet_impl->processEvent(event);
    }
}