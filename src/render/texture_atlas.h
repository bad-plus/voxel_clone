#pragma once

struct LoadedTexture {
    const char* file_path;
    int width;
    int height;
    int channels;
    unsigned char* data;
};

struct AtlasTexture {
    int pixel_x;
    int pixel_y;
    float u1, v1, u2, v2;
};


class TextureAtlas {
public:
    TextureAtlas();
    ~TextureAtlas();

    AtlasTexture pushTextureToAltas(LoadedTexture& texture);

    void update();
    void bind();
private:
    unsigned char* m_atlas_data;
    unsigned int m_atlas_texture_count;
    unsigned int m_gpu_texture_id;
};