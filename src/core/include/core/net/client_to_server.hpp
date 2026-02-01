#pragma once
#include "packet.h"

class PacketServerInfo : public Packet {
public:
    PacketServerInfo() : Packet(PacketType::SERVER_INFO) {}

    void serialize(PacketBuffer& buffer) const override {
        
    }

    void deserialize(PacketBuffer& buffer) override {
        
    }
};