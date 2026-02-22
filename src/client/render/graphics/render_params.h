#pragma once

class RenderParams {
public:
	static RenderParams* getInstance();

	RenderParams(const RenderParams&) = delete;
	RenderParams& operator=(const RenderParams&) = delete;

	void setup() const;

	void depthTest(bool state) const;
	void depthMask(bool state) const;
	void blend(bool state) const;
private:
	static RenderParams* m_instance;
	
	RenderParams();
	~RenderParams();
};