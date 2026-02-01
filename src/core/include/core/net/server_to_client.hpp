#pragma once
#include "packet.h"

class PacketServerInfoResponse : public Packet {
public:
    std::string m_server_name;
    int32_t m_players_count;
    int32_t m_max_players;

    PacketServerInfoResponse()
        : Packet(PacketType::SERVER_INFO_RESPONSE)
        , m_players_count(0)
        , m_max_players(0) {
    }

    void serialize(PacketBuffer& buffer) const override {
        buffer.write_string(m_server_name);
        buffer.write_i32(m_players_count);
        buffer.write_i32(m_max_players);
    }

    void deserialize(PacketBuffer& buffer) override {
        m_server_name = buffer.read_string();
        m_players_count = buffer.read_i32();
        m_max_players = buffer.read_i32();
    }
};