#include "texture2D.h"
#include <core/logger.hpp>

#include "stb_image.h"

#include <GLFW/glfw3.h>

Texture2D::Texture2D(const char* path) {
    glGenTextures(1, &m_texture_id);

    int width, height, channels_count;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &channels_count, 0);

    if(data) {
        GLenum format = GL_RGB;

        if(channels_count == 1) format = GL_RED;
        if(channels_count == 2) LOG_WARN("Bad texture format {0}", path);
        if(channels_count == 3) format = GL_RGB;
        if(channels_count == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        updateParam(GL_TEXTURE_WRAP_S, GL_REPEAT);
        updateParam(GL_TEXTURE_WRAP_T, GL_REPEAT);
        updateParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        updateParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        LOG_ERROR("Failed to load texture: {0}", path);
    }
}
void Texture2D::updateParam(GLenum pname, GLint param) {
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glTexParameteri(GL_TEXTURE_2D, pname, param);
}
Texture2D::~Texture2D() {
    glDeleteTextures(1, &m_texture_id);
}

void Texture2D::bind() {
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
}