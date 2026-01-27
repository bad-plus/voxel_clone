#pragma once
#include <atomic>
#include <memory>
#include <vector>
#include <thread>
#include <string>

struct NetServer;

class Server {
public:
	Server();
	~Server();

	void run();
	void shutdown();

	void runCommandsHandler();
private:
	void handleCommand(std::string_view cmd);

	std::atomic<bool> m_stop;

	std::unique_ptr<NetServer> m_net_server;
	
	std::vector<std::thread> m_threads;
};