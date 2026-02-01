#include "server.h"
#include "../net/net_server.h"
#include <core/logger.hpp>
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
		// TODO: test
		LOG_INFO("Handeled packet: {0} from {1}", (int)packet.getType(), client_id); 
		if (packet.getType() == PacketType::SERVER_INFO) {
			LOG_INFO("Detect packet server info");
			auto packet = PacketServerInfoResponse();
			packet.m_server_name = "Test server name";
			packet.m_max_players = 666;
			m_net_server->sendPacket(client_id, packet);
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
