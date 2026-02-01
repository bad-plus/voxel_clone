#pragma once
#include <cstdint>
#include <core/constants.h>
#include <atomic>
#include <unordered_map>
#include <core/net/packet.h>
#include <functional>

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

    void sendBytes(uint32_t client_id, const void* data, size_t size, bool reliable = true);
    void broadcastBytes(const void* data, size_t size, bool reliable = true);

    void sendPacket(uint32_t client_id, const Packet& packet, bool reliable = true);
    void broadcastPacket(const Packet& packet, bool reliable = true);

    std::function <void(uint32_t client_id, const Packet&)> handlePacketCallback;
private:

    void handleEvent(const ENetEvent& event);

    std::atomic<bool> m_stop;
    ENetHost* m_host;

    std::unordered_map<uint32_t, ENetPeer*> m_clients;
};