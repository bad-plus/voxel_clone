#pragma once
#include <string>
#include <cstdint>
#include <memory>

class Client {
public:
	Client();
	~Client();

	void connect(const std::string& addr, uint16_t port);
	void process();
private:
	struct Impl;
	std::unique_ptr<Impl> m_enet_impl;
};