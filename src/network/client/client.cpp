#define ENET_IMPLEMENTATION
#include <enet/enet.h>
#include <stdexcept>

#include "client.h"
#include "../enet_context.h"
#include "../../core/logger.h"

struct Client::Impl {
    ENetHost* host = nullptr;
    ENetPeer* peer = nullptr;

    ~Impl() {
        if (peer) {
            enet_peer_disconnect(peer, 0);
            enet_peer_reset(peer);
        }
        if (host) {
            enet_host_destroy(host);
        }
    }

    void processEvent(ENetEvent& event) {
        LOG_INFO("[CLIENT] process event: {0}", static_cast<int>(event.type));
    }
};

Client::Client() : m_enet_impl(std::make_unique<Impl>()) {
    ENetContext::instance();
}

Client::~Client() {

}

void Client::connect(const std::string& addr, uint16_t port) {
    m_enet_impl->host = enet_host_create(
        nullptr,
        1,
        2,
        0,
        0
    );

    if (!m_enet_impl->host) {
        throw std::runtime_error("Failed to create ENet client host.");
    }

    ENetAddress address;
    enet_address_set_host(&address, addr.c_str());
    address.port = port;

    m_enet_impl->peer = enet_host_connect(m_enet_impl->host, &address, 2, 0);

    if (!m_enet_impl->peer) {
        throw std::runtime_error("No available peers for initiating an ENet connection.");
    }

    ENetEvent event;
    if (enet_host_service(m_enet_impl->host, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        LOG_INFO("[CLIENT] Success connected!");
    }
    else {
        // bad connected
        enet_peer_reset(m_enet_impl->peer);
        LOG_ERROR("Connection failed");
    }
}

void Client::process() {
    ENetEvent event;

    while (enet_host_service(m_enet_impl->host, &event, 0) > 0) {
        m_enet_impl->processEvent(event);
    }
}