#pragma once
#include "core/net/packet.h"
#include "core/net/client_to_server.hpp"
#include "core/net/server_to_client.hpp"

std::unique_ptr<Packet> Packet::create(const uint8_t* data, size_t size) {
    if (size < 1) {
        throw std::runtime_error("Invalid packet size");
    }

    PacketType type = static_cast<PacketType>(data[0]);
    PacketBuffer buffer(data + 1, size - 1);

    std::unique_ptr<Packet> packet;

    switch (type) {
    case PacketType::SERVER_INFO:
        packet = std::make_unique<PacketServerInfo>();
        break;

    case PacketType::SERVER_INFO_RESPONSE:
        packet = std::make_unique<PacketServerInfoResponse>();
        break;

    default:
        throw std::runtime_error("Unknown packet type");
    }

    packet->deserialize(buffer);
    return packet;
}
