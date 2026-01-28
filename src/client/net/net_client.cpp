#include <enet/enet.h>
#include "net_client.h"
#include "enet_context.h"
#include <core/logger.hpp>

#include <stdexcept>

NetClient::NetClient() : 
    m_host(nullptr), 
    m_connected(false)
{
	ENetContext::instance();
}

NetClient::~NetClient()
{
    if (m_host) {
        enet_host_destroy(m_host);
    }
}

bool NetClient::connect(const char* host, uint16_t port, uint32_t timeout)
{
    if (isConnected()) return true;

    m_host = enet_host_create(
        nullptr,
        1,
        2,
        0,
        0
    );

    if (m_host == nullptr) {
        throw std::runtime_error("Failed to create net client");
    }

    ENetAddress address;

    enet_address_set_host(&address, host);
    address.port = port;

    m_peer = enet_host_connect(m_host, &address, 2, 0);

    if (m_peer == nullptr) {
        throw std::runtime_error("Failed to find free peer for connection");
    }

    ENetEvent event;
    if (enet_host_service(m_host, &event, timeout) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        m_connected.store(true);
        return true;
    }
    else {
        enet_peer_reset(m_peer);
        return false;
    }
}

void NetClient::disconnect()
{
    if (!isConnected()) return;
    enet_peer_disconnect(m_peer, 0);
}

bool NetClient::isConnected() const
{
    return m_connected;
}

void NetClient::handleNetEvents()
{
    ENetEvent event;

    while (enet_host_service(m_host, &event, 1000) > 0) {
        handleEvent(event);
    }
}

void NetClient::shutdown()
{
    disconnect();
}

void NetClient::handleEvent(const ENetEvent& event)
{
    switch (event.type) {
    case ENET_EVENT_TYPE_RECEIVE:
        LOG_INFO("Recived from server {0}", (char*)event.packet->data);
        enet_packet_destroy(event.packet);
        break;

    case ENET_EVENT_TYPE_DISCONNECT:
        LOG_INFO("Disconnected from server");
        m_connected.store(false);
        break;

    default:
        break;
    }
}

void NetClient::sendToServer(const void* data, size_t size, bool reliable)
{
    if (!isConnected()) return;

    ENetPacket* packet = enet_packet_create(
        data, size,
        reliable ? ENET_PACKET_FLAG_RELIABLE : 0
    );
    enet_peer_send(m_peer, 0, packet);
}
