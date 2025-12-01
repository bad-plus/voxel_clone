#pragma once
#include <unordered_map>

struct Shader;
struct TextureAtlas;

class Resources {
public:
    Resources();
    ~Resources();

    TextureAtlas* loadTextureAtlas();
    TextureAtlas* getTextureAtlas();

    Shader* loadShader(const char* shader_name, const char* vertex_path, const char* fragment_path);
    Shader* getShader(const char* shader_name);
private:
    std::unordered_map<const char*, Shader*> m_shaders;
    TextureAtlas* m_texture_atlas;
};