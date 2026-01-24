#include "resources.h"
#include "../../core/logger.h"

#include "../../render/graphics/shader.h"
#include "../../render/graphics/texture_atlas.h"
#include "font.h"

Resources::Resources() = default;

Resources::~Resources() {
    m_shaders.clear();
}

Shader* Resources::loadShader(const char* shader_name, const char* vertex_path, const char* fragment_path) {
    m_shaders[shader_name] = std::make_unique<Shader>(vertex_path, fragment_path);
    return m_shaders[shader_name].get();
}
Shader* Resources::getShader(const char* shader_name) {
    return m_shaders[shader_name].get();
}

TextureAtlas* Resources::loadTextureAtlas() {
    m_texture_atlas = std::make_unique<TextureAtlas>();
    m_texture_atlas->update();
    return m_texture_atlas.get();
}
TextureAtlas* Resources::getTextureAtlas() {
    return m_texture_atlas.get();
}

Font* Resources::loadFont(const char* font_name, const char* font_path, int font_size) {
    m_fonts[font_name] = std::make_unique<Font>(font_path, font_size);
    return m_fonts[font_name].get();
}
Font* Resources::getFont(const char* font_name) {
    return m_fonts[font_name].get();
}