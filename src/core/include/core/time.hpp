#pragma once
#include <chrono>

class Time {
	using Clock = std::chrono::steady_clock;
public:
	Time() : m_time(0) {}

	Time(long long time) : m_time(time) { }

	static Time now() {
		return Time(nowNS());
	}

	static Time fromNS(long long ns) { return Time(ns); }
	static Time fromMS(long long ms) { return Time(ms * 1'000'000); }
	static Time fromS(long long s) { return Time(s * 1'000'000'000); }

	static long long nowNS() {
		return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now().time_since_epoch()).count();
	}

	long long getNS() const {
		return m_time;
	}

	template <typename T>
	T getMS() const {
		return static_cast<T>(m_time) / 1'000'000;
	}

	template <typename T>
	T getS() const {
		return static_cast<T>(m_time) / 1'000'000'000;
	}

	Time getElapsedTime() const {
		return Time(Time::nowNS() - m_time);
	}

	Time operator + (const Time& other) const {
		return Time(m_time + other.m_time);
	}

	Time operator - (const Time& other) const {
		return Time(m_time - other.m_time);
	}

	Time operator / (const int value) const {
		return Time(m_time / value);
	}

	bool operator > (const Time& other) const {
		return (m_time > other.m_time);
	}

	bool operator < (const Time& other) const {
		return (m_time < other.m_time);
	}

	bool operator >= (const Time& other) const {
		return (m_time >= other.m_time);
	}

	bool operator <= (const Time& other) const {
		return (m_time <= other.m_time);
	}

	bool operator == (const Time& other) const {
		return (m_time == other.m_time);
	}

	Time& operator = (const Time& other) {
		m_time = other.m_time;
		return *this;
	}

	explicit operator long long() const {
		return m_time;
	}

	Time& operator += (const Time& other) {
		m_time += other.m_time;
		return *this;
	}

	Time& operator -= (const Time& other) {
		m_time -= other.m_time;
		return *this;
	}
private:

	long long m_time;
};