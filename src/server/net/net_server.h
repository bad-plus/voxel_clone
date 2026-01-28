#pragma once
#include <cstdint>
#include <core/constants.h>
#include <atomic>

struct _ENetHost;
typedef _ENetHost ENetHost;

struct _ENetEvent;
typedef _ENetEvent ENetEvent;

struct _ENetPeer;
typedef _ENetPeer ENetPeer;

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
    void sendToPeer(ENetPeer* peer, const void* data, size_t size, bool reliable = true);
    void broadcastPeers(const void* data, size_t size, bool reliable = true);

    void handleEvent(const ENetEvent& event);

    std::atomic<bool> m_stop;
    ENetHost* m_host;
};