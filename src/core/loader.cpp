#include "loader.h"
#include "game.h"
#include "../render/shader.h"
#include "../render/texture_atlas.h"

Loader::Loader(GameContext* context) {
    m_game_context = context;
}
Loader::~Loader() {

}

void Loader::loadResources() {
    loadAtlas();
    loadShaders();
}
void Loader::loadShaders() {
    m_game_context->resources->loadShader("block_shader", "world_block.vert", "world_block.frag");
}
void Loader::loadAtlas() {
    m_game_context->resources->loadTextureAtlas();
}