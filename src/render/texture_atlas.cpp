#include "texture_atlas.h"
#include "../core/logger.h"

#include "../world/block.h"

#include "stb_image.h"
#include "stb_rect_pack.h"
#include "stb_image_resize2.h"
#include "stb_image_write.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <memory>
#include <filesystem>
#include <cstring>

namespace fs = std::filesystem;

#define TEXTURES_PATH               "resources/textures/blocks/"

#define ATLAS_SIZE                  1280// px
#define ATLAS_CHANNELS              4 // color channels

#define ATLAS_TILE_SIZE             64

std::unique_ptr<unsigned char[]> convertToRGBA(
    unsigned char* data, int width, int height, int channels) {
    
    std::unique_ptr<unsigned char[]> rgba_data(
        new unsigned char[width * height * 4]
    );

    for(int i = 0; i < width * height; i++) {
        switch(channels) {
            case 1: // Grayscale -> RGBA
                rgba_data[i * 4 + 0] = data[i];
                rgba_data[i * 4 + 1] = data[i];
                rgba_data[i * 4 + 2] = data[i];
                rgba_data[i * 4 + 3] = 255;
                break;
            case 3: // RGB -> RGBA
                rgba_data[i * 4 + 0] = data[i * 3 + 0];
                rgba_data[i * 4 + 1] = data[i * 3 + 1];
                rgba_data[i * 4 + 2] = data[i * 3 + 2];
                rgba_data[i * 4 + 3] = 255;
                break;
            case 4: // RGBA -> RGBA (просто копируем)
                memcpy(&rgba_data[i * 4], &data[i * 4], 4);
                break;
            default:
                LOG_WARN("Unsupported channel count: {0}", channels);
                rgba_data[i * 4 + 0] = 0;
                rgba_data[i * 4 + 1] = 0;
                rgba_data[i * 4 + 2] = 0;
                rgba_data[i * 4 + 3] = 255;
        }
    }

    return rgba_data;
}

LoadedTexture loadTexture(const std::string path) {
    LoadedTexture tex;
    tex.file_path = path.c_str();
    tex.data = stbi_load(path.c_str(), &tex.width, &tex.height, &tex.channels, 0);

    if(!tex.data) {
        throw std::runtime_error(std::string("Failed to load texture: ") + path);
    }

    // Преобразуем в RGBA если нужно
    if(tex.channels != 4) {
        auto rgba_data = convertToRGBA(tex.data, tex.width, tex.height, tex.channels);
        stbi_image_free(tex.data);
        tex.data = rgba_data.release();
        tex.channels = 4;
    }

    return tex;
}

AtlasTexture TextureAtlas::pushTextureToAltas(LoadedTexture& texture) {
    if(!texture.data)
        throw std::runtime_error(std::string("Texture data is null: ") + texture.file_path);

    AtlasTexture atlas_texture;

    std::unique_ptr<unsigned char[]> resized_texture(
        new unsigned char[ATLAS_TILE_SIZE * ATLAS_TILE_SIZE * ATLAS_CHANNELS]
    );

    auto ok = stbir_resize_uint8_linear(
        texture.data, texture.width, texture.height, 0,
        resized_texture.get(), ATLAS_TILE_SIZE, ATLAS_TILE_SIZE, 0,
        STBIR_RGBA
    );

    if(!ok)
        throw std::runtime_error(std::string("Failed to resize image: ") + texture.file_path);

    const unsigned int cells_per_row = ATLAS_SIZE / ATLAS_TILE_SIZE;

    const unsigned int cell_x = m_atlas_texture_count % cells_per_row;
    const unsigned int cell_y = m_atlas_texture_count / cells_per_row;

    const unsigned int dst_x = cell_x * ATLAS_TILE_SIZE;
    const unsigned int dst_y = cell_y * ATLAS_TILE_SIZE;

    for(unsigned int y = 0; y < ATLAS_TILE_SIZE; y++) {
        unsigned char* dst_row =
            m_atlas_data + ((dst_y + y) * ATLAS_SIZE + dst_x) * ATLAS_CHANNELS;
        const unsigned char* src_row =
            resized_texture.get() + (y * ATLAS_TILE_SIZE) * ATLAS_CHANNELS;
        
        memcpy(dst_row, src_row, ATLAS_TILE_SIZE * ATLAS_CHANNELS);
    }

    atlas_texture.pixel_x = dst_x;
    atlas_texture.pixel_y = dst_y;

    atlas_texture.u1 = float(dst_x) / ATLAS_SIZE;
    atlas_texture.v1 = float(dst_y) / ATLAS_SIZE;

    atlas_texture.u2 = float(dst_x + ATLAS_TILE_SIZE) / ATLAS_SIZE;
    atlas_texture.v2 = float(dst_y + ATLAS_TILE_SIZE) / ATLAS_SIZE;

    m_atlas_texture_count++;

    return atlas_texture;
}

inline void atlasTextureToSideUV(const AtlasTexture& t, SideUV* outUV) {
    outUV->uv[0] = {t.u1, t.v2};
    outUV->uv[1] = {t.u2, t.v2};
    outUV->uv[2] = {t.u2, t.v1};
    outUV->uv[3] = {t.u1, t.v1};
}

struct BlockTextureFiles {
    std::string top, bottom, side;
};

BlockTextureFiles getBlockTextureFiles(const std::string& block_name) {
    BlockTextureFiles tex;
    std::string base = TEXTURES_PATH + block_name;

    if(fs::exists(base + "_top.png")) tex.top = base + "_top.png";
    if(fs::exists(base + "_bottom.png")) tex.bottom = base + "_bottom.png";
    if(fs::exists(base + "_side.png")) tex.side = base + "_side.png";

    if(tex.top.empty() && fs::exists(base + ".png")) tex.top = base + ".png";
    if(tex.bottom.empty() && fs::exists(base + ".png")) tex.bottom = base + ".png";
    if(tex.side.empty() && fs::exists(base + ".png")) tex.side = base + ".png";

    return tex;
}

void TextureAtlas::update() {
    m_atlas_texture_count = 0;
    
    memset(m_atlas_data, 0, ATLAS_SIZE * ATLAS_SIZE * ATLAS_CHANNELS);

    for(auto& block_info : BlocksInfo) {
        try {
            if(block_info.first == BlockID::EMPTY) continue;

            BlockTexture& block_texture = block_info.second.texture;

            BlockTextureFiles texture_files = getBlockTextureFiles(block_info.second.name);

            std::unordered_map<std::string, AtlasTexture*> loaded_map;

            auto pushIfExists = [&](const std::string& path) -> AtlasTexture* {
                if(path.empty()) return nullptr;

                auto it = loaded_map.find(path);
                if(it != loaded_map.end()) return it->second;

                LoadedTexture loaded = loadTexture(path);
                AtlasTexture atlasTex = pushTextureToAltas(loaded);
                stbi_image_free(loaded.data);

                AtlasTexture* ptr = new AtlasTexture(atlasTex);
                loaded_map[path] = ptr;
                return ptr;
            };

            AtlasTexture* side_tex = pushIfExists(texture_files.side);
            for(int s : { (int)BlockSide::LEFT, (int)BlockSide::RIGHT, (int)BlockSide::FRONT, (int)BlockSide::BACK }) {
                if(side_tex) atlasTextureToSideUV(*side_tex, &block_texture.sides[s]);
            }

            AtlasTexture* top_tex = pushIfExists(texture_files.top);
            if(top_tex) atlasTextureToSideUV(*top_tex, &block_texture.sides[(int)BlockSide::TOP]);

            AtlasTexture* bottom_tex = pushIfExists(texture_files.bottom);
            if(bottom_tex) atlasTextureToSideUV(*bottom_tex, &block_texture.sides[(int)BlockSide::BOTTOM]);

            for(auto& pair : loaded_map) delete pair.second;

        } catch(const std::runtime_error& e) {
            LOG_ERROR(e.what());
        }
    }

    unsigned int new_gpu_texture_id = 0;

    glGenTextures(1, &new_gpu_texture_id);

    glBindTexture(GL_TEXTURE_2D, new_gpu_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ATLAS_SIZE, ATLAS_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_atlas_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned int old_gpu_texture_id = m_gpu_texture_id;
    m_gpu_texture_id = new_gpu_texture_id;
    glDeleteTextures(1, &old_gpu_texture_id);

    LOG_INFO("Texture atlas generated! {0} textures", m_atlas_texture_count);
}

TextureAtlas::TextureAtlas() {
    m_atlas_data = new unsigned char[ATLAS_SIZE * ATLAS_SIZE * ATLAS_CHANNELS];
    memset(m_atlas_data, 0, ATLAS_SIZE * ATLAS_SIZE * ATLAS_CHANNELS);
}

TextureAtlas::~TextureAtlas() {
    delete[] m_atlas_data;
    glDeleteTextures(1, &m_gpu_texture_id);
}

void TextureAtlas::bind() {
    glBindTexture(GL_TEXTURE_2D, m_gpu_texture_id);
}