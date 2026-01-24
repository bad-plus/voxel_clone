#pragma once
#include <glad/glad.h>

class Texture2D {
public:
    Texture2D(const char* path);
    void updateParam(GLenum pname, GLint param);
    ~Texture2D();

    void bind();
private:
    unsigned int m_texture_id;
};