#pragma once
#include <core/time.hpp>

class FrameCounter {
public:
	FrameCounter();

	void frame();

	int getFPS() const;
private:
	int m_fps;
	int m_frames_count;
	Time m_prev_fps_time;
};