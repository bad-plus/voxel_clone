#pragma once

class FrameCounter {
public:
	FrameCounter();

	void frame();

	int getFPS() const;
private:
	int m_fps;
	int m_frames_count;
	int m_prev_fps_time;
};