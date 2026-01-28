#pragma once
#include <memory>
#include <thread>
#include <vector>
#include <core/constants.h>

struct NetClient;

class Client {
public:
	Client();
	~Client();

	bool connect(const char* host, uint16_t port = Constants::DEFAULT_SERVER_PORT);
	void disconnect();
private:
	void runNetHandler();

	std::unique_ptr<NetClient> m_net_client;

	std::vector<std::thread> m_threads;
};