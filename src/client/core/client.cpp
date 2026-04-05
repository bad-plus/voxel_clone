#include "client.h"
#include "../net/net_client.h"
#include <core/net/client_to_server.hpp>
#include <core/net/server_to_client.hpp>
#include <core/logger.hpp>
#include "../world/client_world.h"
#include <memory>
#include "../world/events/client_events.h"
#include "../utils/circle_around_point.h"
#include <algorithm>
#include <core/ecs/core/ecs.h>

Client::Client() : 
    m_net_client(std::make_unique<NetClient>()),
    m_world(std::make_unique<ClientWorld>())
{
    // TODO: test
    m_net_client->handlePacketCallback = [&](const Packet& packet) {
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
            LOG_INFO("Received chunk: {0} {1} (size {2})",
                response->m_chunk_position.x,
                response->m_chunk_position.z,
                response->m_chunk_storage.size());

            m_world->addEvent(
                std::make_unique<LoadChunkFromServerEvent>(
                    response->m_chunk_position,
                    response->m_chunk_storage),
                true);
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
    m_world->shutdown();
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
        m_threads.emplace_back(&Client::runWorldChunksManager, this);

        // TODO: test
        auto packet = PacketServerInfo();
        m_net_client->sendPacket(packet);

        auto packet2 = PacketGetChunk();
        packet2.m_chunk_position.x = 0;
        packet2.m_chunk_position.z = 0;

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

ClientWorld* Client::getWorld() const
{
    return m_world.get();
}

Entity Client::getPlayerEntity() const
{
    return m_player_entity;
}

void Client::setPlayerEntity(Entity entity)
{
    m_player_entity = entity;
}

void Client::runNetHandler()
{
    while (m_net_client->isConnected()) {
        m_net_client->handleNetEvents();
    }
}

void Client::runWorldChunksManager()
{
    while (m_net_client->isConnected()) {
        std::erase_if(m_responsed_chunks, [&](const auto& kv) {
            ChunkCoord coord = ChunkCoord::ChunkCoord(kv.first);
            return m_world->getChunk(coord.x, coord.z) != nullptr;
            });

        std::erase_if(m_responsed_chunks, [](const auto& kv) {
            return Time::now() - kv.second > Time::fromS(5);
            });

        if (m_responsed_chunks.size() > 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        const auto& player_transform =
            m_world->getECS()->storage<Transform>().get(m_player_entity);

        int chunk_offset_x = static_cast<int>(
            std::floor(player_transform.position.x / Constants::CHUNK_SIZE_X));
        int chunk_offset_z = static_cast<int>(
            std::floor(player_transform.position.z / Constants::CHUNK_SIZE_Z));

        auto chunks_around_entity = Utils::circleAroundPoint(
            chunk_offset_x, chunk_offset_z, Constants::MAX_LOAD_CHUNKS_DIST);

        for (const auto& [x, z] : chunks_around_entity) {
            if (m_responsed_chunks.size() > 16) continue;
            if (m_world->getChunk(x, z) != nullptr) continue;

            long long index = ChunkCoord(x, z).getIndex();

            if (m_responsed_chunks.contains(index)) continue;

            auto packet = PacketGetChunk();
            packet.m_chunk_position.x = x;
            packet.m_chunk_position.z = z;
            m_net_client->sendPacket(packet);

            m_responsed_chunks[index] = Time::now();
            LOG_INFO("Send request to server {0} {1}", x, z);

        }
    }
}