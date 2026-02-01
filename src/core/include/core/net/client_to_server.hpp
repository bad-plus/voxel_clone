#pragma once
#include "packet.h"
#include <core/world/chunk/chunk.h>

class PacketServerInfo : public Packet {
public:
    PacketServerInfo() : Packet(PacketType::SERVER_INFO) {}

    void serialize(PacketBuffer& buffer) const override {
        
    }

    void deserialize(PacketBuffer& buffer) override {
        
    }
};

class PacketGetChunk : public Packet {
public:
    ChunkCoord m_chunk_position;

    PacketGetChunk() : 
        Packet(PacketType::GET_CHUNK) {

    }

    void serialize(PacketBuffer& buffer) const override {
        buffer.write_i32(m_chunk_position.x);
        buffer.write_i32(m_chunk_position.z);
    }

    void deserialize(PacketBuffer& buffer) override {
        m_chunk_position.x = buffer.read_i32();
        m_chunk_position.z = buffer.read_i32();
    }
};