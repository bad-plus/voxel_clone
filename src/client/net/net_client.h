#pragma once
#include <string>
#include <atomic>
#include <functional>
#include <memory>
#include <core/net/packet.h>

struct _ENetHost;
typedef _ENetHost ENetHost;

struct _ENetEvent;
typedef _ENetEvent ENetEvent;

struct _ENetPeer;
typedef _ENetPeer ENetPeer;

class NetClient {
public:
	NetClient();
	~NetClient();

	bool connect(const char* host, uint16_t port, uint32_t timeout = 5000);
	void disconnect();

	bool isConnected() const;

	void handleNetEvents();
	void shutdown();

	void sendPacket(const Packet& packet, bool reliable = true);
	std::function <void(const Packet&)> handlePacketCallback;
private:
	void handleEvent(const ENetEvent& event);

	void sendBytes(const void* data, size_t size, bool reliable = true);

	ENetHost* m_host;
	ENetPeer* m_peer;

	std::atomic<bool> m_connected;
	
};