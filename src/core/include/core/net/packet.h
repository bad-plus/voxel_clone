#pragma once
#include <cstdint>
#include <stdexcept>
#include <memory>

#include "packet_buffer.hpp"

enum class PacketType : uint8_t {
	// Client -> Server
	SERVER_INFO,

	// Server -> Client
	SERVER_INFO_RESPONSE,
};

class Packet {
public:
	Packet(PacketType type) : m_type(type) {}
	virtual ~Packet() = default;

	virtual void serialize(PacketBuffer& buffer) const = 0;
	virtual void deserialize(PacketBuffer& buffer) = 0;

	PacketType getType() const { return m_type; }

	std::vector<uint8_t> to_bytes() const {
		PacketBuffer buffer;
		buffer.write_u8(static_cast<uint8_t>(m_type));
		serialize(buffer);
		return std::vector<uint8_t>(buffer.data(), buffer.data() + buffer.size());
	}

    static std::unique_ptr<Packet> create(const uint8_t* data, size_t size);
protected:
	PacketType m_type;
};