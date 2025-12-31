#include "loader.h"
#include "../../core/game.h"
#include "../../render/graphics/shader.h"
#include "../../render/graphics/texture_atlas.h"
#include "resources.h"
#include "font.h"

Loader::Loader(Resources* resources) {
    m_resources = resources;
}
Loader::~Loader() = default;

void Loader::loadResources() {
    loadAtlas();
    loadShaders();
    loadFonts();
}
void Loader::loadShaders() {
    m_resources->loadShader("block_shader", "world_block.vert", "world_block.frag");
    m_resources->loadShader("text_shader", "text.vert", "text.frag");
    m_resources->loadShader("crosshair", "crosshair.vert", "crosshair.frag");
}
void Loader::loadAtlas() {
    m_resources->loadTextureAtlas();
}

void Loader::loadFonts() {
    m_resources->loadFont("arial_22", "arial.ttf", 22);
}