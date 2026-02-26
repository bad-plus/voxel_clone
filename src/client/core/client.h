#pragma once
#include <memory>
#include <thread>
#include <vector>
#include <core/constants.h>
#include <core/ecs/core/entity.h>

struct NetClient;
struct ClientWorld;

class Client {
public:
	Client();
	~Client();

	bool connect(const char* host, uint16_t port = Constants::DEFAULT_SERVER_PORT);
	void disconnect();

	ClientWorld* getWorld() const;
	Entity getPlayerEntity() const;

	void setPlayerEntity(Entity entity);
private:
	void runNetHandler();

	std::unique_ptr<NetClient> m_net_client;
	std::unique_ptr<ClientWorld> m_world;

	Entity m_player_entity;

	std::vector<std::thread> m_threads;
};