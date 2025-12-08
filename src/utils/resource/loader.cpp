#include "loader.h"
#include "../../core/game.h"
#include "../../render/graphics/shader.h"
#include "../../render/graphics/texture_atlas.h"

Loader::Loader(Resources* resources) {
    m_resources = resources;
}
Loader::~Loader() {

}

void Loader::loadResources() {
    loadAtlas();
    loadShaders();
}
void Loader::loadShaders() {
    m_resources->loadShader("block_shader", "world_block.vert", "world_block.frag");
}
void Loader::loadAtlas() {
    m_resources->loadTextureAtlas();
}