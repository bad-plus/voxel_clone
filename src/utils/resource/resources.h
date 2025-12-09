#pragma once
#include <unordered_map>
#include <string>

struct Shader;
struct TextureAtlas;
struct Font;

class Resources {
public:
    Resources();
    ~Resources();

    TextureAtlas* loadTextureAtlas();
    TextureAtlas* getTextureAtlas();

    Shader* loadShader(const char* shader_name, const char* vertex_path, const char* fragment_path);
    Shader* getShader(const char* shader_name);

    Font* loadFont(const char* font_name, const char* font_path, int font_size);
    Font* getFont(const char* font_name);
private:
    std::unordered_map<std::string, Shader*> m_shaders;
    std::unordered_map<std::string, Font*> m_fonts;
    TextureAtlas* m_texture_atlas;
};