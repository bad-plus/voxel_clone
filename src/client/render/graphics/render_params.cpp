#include "render_params.h"

#include <glad/glad.h>

RenderParams* RenderParams::m_instance = nullptr;

RenderParams* RenderParams::getInstance()
{
    if (m_instance == nullptr)
        m_instance = new RenderParams();

    return m_instance;
}

void RenderParams::setup() const
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderParams::depthTest(bool state) const
{
    static bool current_state = false;
    if (current_state != state) {
        if (state) {
            glEnable(GL_DEPTH_TEST);
        }
        else {
            glDisable(GL_DEPTH_TEST);
        }
        current_state = state;
    }
}

void RenderParams::depthMask(bool state) const
{
    static bool current_state = false;
    if (current_state != state) {
        if (state) {
            glDepthMask(GL_TRUE);
        }
        else {
            glDepthMask(GL_FALSE);
        }
        current_state = state;
    }
}

void RenderParams::blend(bool state) const
{
    static bool current_state = false;
    if (current_state != state) {
        if (state) {
            glEnable(GL_BLEND);
        }
        else {
            glDisable(GL_BLEND);
        }
        current_state = state;
    }
}

RenderParams::RenderParams()
{
}

RenderParams::~RenderParams()
{
}