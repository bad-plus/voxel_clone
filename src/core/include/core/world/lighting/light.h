#pragma once
#include <cstdint>

using light_t = uint16_t;

class Light {
public:
	Light() = default;
	Light(int r, int g, int b, int s) {
		set(r, g, b, s);
	}

	void set(int r, int g, int b, int s) {
		m_data = (r & 0xF) | ((g & 0xF) << 4) | ((b & 0xF) << 8) | ((s & 0xF) << 12);
	}

	void setR(int r) { m_data = (m_data & 0xFFF0) | (r & 0xF); }
	void setG(int g) { m_data = (m_data & 0xFF0F) | ((g & 0xF) << 4); }
	void setB(int b) { m_data = (m_data & 0xF0FF) | ((b & 0xF) << 8); }
	void setS(int s) { m_data = (m_data & 0x0FFF) | ((s & 0xF) << 12); }

	int getR() const { return m_data & 0xF; }
	int getG() const { return (m_data >> 4) & 0xF; }
	int getB() const { return (m_data >> 8) & 0xF; }
	int getS() const { return (m_data >> 12) & 0xF; }

	light_t getData() const { return m_data; }
	void setData(light_t data) { m_data = data; }

	static inline light_t getColorData(const int r, const int g, const int b, const int s = 0) {
		return light_t((r & 0xF) | ((g & 0xF) << 4) | ((b & 0xF) << 8) | ((s & 0xF) << 12));
	}

	float getAsFloat() const {
		uint32_t data32 = static_cast<uint32_t>(m_data);
		float result;
		std::memcpy(&result, &data32, sizeof(float));
		return result;
	}
private:
	light_t m_data = 0;
};