#include "frame_counter.h"

#include <glfw/glfw3.h>

FrameCounter::FrameCounter() {
	m_fps = 0;
	m_frames_count = 0;
	m_prev_fps_time = 0;
}

void FrameCounter::frame() {
	int current_time = static_cast<int>(glfwGetTime());

	if (current_time != m_prev_fps_time) {
		m_fps = m_frames_count;
		m_prev_fps_time = current_time;
		m_frames_count = 0;
	}
	else {
		m_frames_count++;
	}
}

int FrameCounter::getFPS() const {
	return m_fps;
}