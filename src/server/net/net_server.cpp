#include <enet/enet.h>
#include "net_server.h"
#include "enet_context.h"

#include <core/logger.hpp>

NetServer::NetServer() : m_stop(true), m_host(nullptr)
{
    ENetContext::instance();
}

NetServer::~NetServer()
{
    if (m_host) {
        enet_host_destroy(m_host);
    }
    
    LOG_INFO("NetServer stoped");
}

void NetServer::run(size_t max_clients, uint16_t port)
{
    if (!m_stop) return;
    m_stop.store(false);

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    m_host = enet_host_create(
        &address,
        max_clients,
        2,
        0,
        0
    );

    if (m_host == nullptr) {
        throw std::runtime_error("Failed to create net server");
    }

    LOG_INFO("NetServer started in {} port", port);
}

void NetServer::shutdown()
{
    m_stop.store(true);
}

void NetServer::sendToPeer(ENetPeer* peer, const void* data, size_t size, bool reliable)
{
    ENetPacket* packet = enet_packet_create(
        data, size,
        reliable ? ENET_PACKET_FLAG_RELIABLE : 0
    );
    enet_peer_send(peer, 0, packet);
}

void NetServer::broadcastPeers(const void* data, size_t size, bool reliable)
{
    ENetPacket* packet = enet_packet_create(
        data, size,
        reliable ? ENET_PACKET_FLAG_RELIABLE : 0
    );
    enet_host_broadcast(m_host, 0, packet);
}

void NetServer::runNetHandler()
{
    ENetEvent event;

    while (!m_stop) {
        while (enet_host_service(m_host, &event, 1000) > 0) {
            handleEvent(event);
        }
    }
}

void NetServer::handleEvent(const ENetEvent& event)
{
    switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
        {
            LOG_INFO("New connection {0}:{1}\n",
                event.peer->address.host,
                event.peer->address.port);

            event.peer->data = (void*)"Client info";


            const char* welcome = "Welcome to server!";
            sendToPeer(event.peer, welcome, strlen(welcome) + 1);
            break;
        }

        case ENET_EVENT_TYPE_RECEIVE:
        {
            LOG_INFO("Recived packet {0} bytes, channel {1}\n",
                event.packet->dataLength,
                event.channelID);

            char* data = (char*)event.packet->data;
            LOG_INFO("Message: {0}\n", data);

            const char* response = "Message received!";
            sendToPeer(event.peer, response, strlen(response) + 1);

            enet_packet_destroy(event.packet);
            break;
        }

        case ENET_EVENT_TYPE_DISCONNECT:
        {
            LOG_INFO("Disconnect client {0}:{1}\n",
                event.peer->address.host,
                event.peer->address.port);

            event.peer->data = nullptr;
            break;
        }
        case ENET_EVENT_TYPE_NONE:
            break;
    }
}
