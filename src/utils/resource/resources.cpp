#include "resources.h"
#include "../../core/logger.h"

#include "../../render/graphics/shader.h"
#include "../../render/graphics/texture_atlas.h"
#include "font.h"

Resources::Resources() {
    
}
Resources::~Resources() {
    for(auto& shader : m_shaders) {
        delete shader.second;
    }
    m_shaders.clear();

    delete m_texture_atlas;
}

Shader* Resources::loadShader(const char* shader_name, const char* vertex_path, const char* fragment_path) {
    m_shaders[shader_name] = new Shader(vertex_path, fragment_path);
    return m_shaders[shader_name];
}
Shader* Resources::getShader(const char* shader_name) {
    return m_shaders[shader_name];
}

TextureAtlas* Resources::loadTextureAtlas() {
    m_texture_atlas = new TextureAtlas;
    m_texture_atlas->update();
    return m_texture_atlas;
}
TextureAtlas* Resources::getTextureAtlas() {
    return m_texture_atlas;
}

Font* Resources::loadFont(const char* font_name, const char* font_path, int font_size) {
    m_fonts[font_name] = new Font(font_path, font_size);
    return m_fonts[font_name];
}
Font* Resources::getFont(const char* font_name) {
    return m_fonts[font_name];
}