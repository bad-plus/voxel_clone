#include "server.h"
#include "../net/net_server.h"
#include <core/logger.hpp>

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
