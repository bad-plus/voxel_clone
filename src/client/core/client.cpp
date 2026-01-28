#include "client.h"
#include "../net/net_client.h"
#include <core/logger.hpp>

Client::Client() : m_net_client(std::make_unique<NetClient>())
{

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
	}
	else {
		LOG_INFO("Failed connect to server");
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
