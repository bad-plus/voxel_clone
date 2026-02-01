#pragma once
#include <cstdint>
#include <stdexcept>
#include <vector>

class PacketBuffer {
public:
	PacketBuffer() : m_read_pos(0) {}
	PacketBuffer(const uint8_t* data, size_t size)
		: m_data(data, data + size), m_read_pos(0) {
	}

	void write_u8(uint8_t value) {
		m_data.push_back(value);
	}

	void write_u16(uint16_t value) {
		m_data.push_back(value & 0xFF);
		m_data.push_back((value >> 8) & 0xFF);
	}

	void write_u32(uint32_t value) {
		m_data.push_back(value & 0xFF);
		m_data.push_back((value >> 8) & 0xFF);
		m_data.push_back((value >> 16) & 0xFF);
		m_data.push_back((value >> 24) & 0xFF);
	}

	void write_i32(int32_t value) {
		write_u32(static_cast<uint32_t>(value));
	}

	void write_string(const std::string& str) {
		write_u16(static_cast<uint16_t>(str.size()));
		m_data.insert(m_data.end(), str.begin(), str.end());
	}

	uint8_t read_u8() {
		if (m_read_pos >= m_data.size())
			throw std::runtime_error("Buffer overflow");
		return m_data[m_read_pos++];
	}

	uint8_t read_u16() {
		uint16_t value = read_u8();
		value |= static_cast<uint16_t>(read_u8()) << 8;
		return value;
	}

	uint32_t read_u32() {
		uint32_t value = read_u8();
		value |= static_cast<uint32_t>(read_u8()) << 8;
		value |= static_cast<uint32_t>(read_u8()) << 16;
		value |= static_cast<uint32_t>(read_u8()) << 24;
		return value;
	}

	int32_t read_i32() {
		return static_cast<int32_t>(read_u32());
	}

	std::string read_string() {
		uint16_t length = read_u16();
		if (m_read_pos + length > m_data.size())
			throw std::runtime_error("Buffer overflow");

		std::string result(m_data.begin() + m_read_pos,
			m_data.begin() + m_read_pos + length);
		m_read_pos += length;
		return result;
	}

	const uint8_t* data() const { return m_data.data(); }
	size_t size() const { return m_data.size(); }

	void clear() {
		m_data.clear();
		m_read_pos = 0;
	}
private:
	std::vector<uint8_t> m_data;
	size_t m_read_pos;
};
