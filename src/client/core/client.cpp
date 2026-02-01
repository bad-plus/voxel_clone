#include "client.h"
#include "../net/net_client.h"
#include <core/net/client_to_server.hpp>
#include <core/net/server_to_client.hpp>
#include <core/logger.hpp>

Client::Client() : m_net_client(std::make_unique<NetClient>())
{
    // TODO: test
    m_net_client->handlePacketCallback = [](const Packet& packet) {
    switch (packet.getType()) {
        case PacketType::SERVER_INFO_RESPONSE: {
            auto* response = static_cast<const PacketServerInfoResponse*>(&packet);
            LOG_INFO("Server: {0} ({1} players)",
                response->m_server_name,
                response->m_players_count);
            break;
        }
        case PacketType::GET_CHUNK_RESPONSE: {
            auto* response = static_cast<const PacketGetChunkResponse*>(&packet);
            
            std::string data;
            for (auto i : response->m_chunk_storage) {
                data += std::to_string(i) + " ";
            }
            LOG_INFO("Receive chunk: {0} {1} (size {2})\nData: {3}",
                response->m_chunk_position.x,
                response->m_chunk_position.z,
                response->m_chunk_storage.size(),
                data);
            break;
        }
        
        default:
            LOG_WARN("Unknown packet type: {0}", (int)packet.getType());
            break;
        }
    };
}

Client::~Client()
{
    disconnect();
    for (auto& thread : m_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

bool Client::connect(const char* host, uint16_t port)
{
    bool status = m_net_client->connect(host, port);
    if (status) {
        LOG_INFO("Success connected to server");
        m_threads.emplace_back(&Client::runNetHandler, this);

        // TODO: test
        auto packet = PacketServerInfo();
        m_net_client->sendPacket(packet);

        auto packet2 = PacketGetChunk();
        packet2.m_chunk_position.x = 333;
        packet2.m_chunk_position.z = -333;

        m_net_client->sendPacket(packet2);
    }
    else {
        LOG_ERROR("Failed connect to server");
    }
    return status;
}

void Client::disconnect()
{
    m_net_client->disconnect();
}

void Client::runNetHandler()
{
    while (m_net_client->isConnected()) {
        m_net_client->handleNetEvents();
    }
}