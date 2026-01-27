#pragma once
#include <cstdint>
#include <core/constants.h>
#include <atomic>

struct _ENetHost;
typedef _ENetHost ENetHost;

struct _ENetEvent;
typedef _ENetEvent ENetEvent;

class NetServer {
public:
    NetServer();
    ~NetServer();

    void run(
        size_t max_clients = Constants::DEFAULT_MAX_CLIENTS,
        uint16_t port = Constants::DEFAULT_SERVER_PORT
    );
    void runNetHandler();

    void shutdown();
private:
    void handleEvent(const ENetEvent& event);

    std::atomic<bool> m_stop;
    ENetHost* m_host;
};