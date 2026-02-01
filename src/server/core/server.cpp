#include "server.h"
#include "../net/net_server.h"
#include <core/logger.hpp>
#include <core/net/client_to_server.hpp>
#include <core/net/server_to_client.hpp>

#include <iostream>

Server::Server() : 
	m_stop(true),
	m_net_server(std::make_unique<NetServer>())
{

}

Server::~Server()
{
	shutdown();

	for (auto& thread : m_threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}

void Server::run()
{
	if (!m_stop) return;
	m_stop.store(false);

	m_net_server->run();

    m_net_server->handlePacketCallback = [this](uint32_t client_id, const Packet& packet) {
        LOG_INFO("Handled packet: {0} from {1}", (int)packet.getType(), client_id);

        switch (packet.getType()) {
        case PacketType::SERVER_INFO: {
            LOG_INFO("Detect packet server info");
            auto response_packet = PacketServerInfoResponse();
            response_packet.m_server_name = "Test server name";
            response_packet.m_max_players = 666;
            m_net_server->sendPacket(client_id, response_packet);
            break;
        }

        case PacketType::GET_CHUNK: {
            auto* chunk_request = static_cast<const PacketGetChunk*>(&packet);

            LOG_INFO("Detect packet get chunk ( {0} , {1} )", chunk_request->m_chunk_position.x, chunk_request->m_chunk_position.z);

            auto response_packet = PacketGetChunkResponse();
            response_packet.m_chunk_position = chunk_request->m_chunk_position;
			for (int i = 0; i < 555; i++) {
				//test data
				response_packet.m_chunk_storage.push_back(i % 255);
			}

            m_net_server->sendPacket(client_id, response_packet);
            break;
        }

        default:
            LOG_WARN("Unknown packet type: {0}", (int)packet.getType());
            break;
        }
    };

	m_threads.emplace_back(&NetServer::runNetHandler, m_net_server.get());
	m_threads.emplace_back(&Server::runCommandsHandler, this);

	LOG_INFO("Server success started");

	for (auto& thread : m_threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}

void Server::shutdown()
{
	m_net_server->shutdown();
	m_stop = true;
}

void Server::runCommandsHandler()
{
	while (!m_stop) {
		std::cout << "cmd: ";
		std::string cmd;

		std::getline(std::cin, cmd);

		handleCommand(cmd);
	}
}

void Server::handleCommand(std::string_view cmd)
{
	if (cmd == "exit") {
		shutdown();
	}
}
