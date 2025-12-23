#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

enum class BlockID { 
    EMPTY = 0,
    ERROR,
    STONE,
    DIRT,
    GRASS,
    SNOW_GRASS,
    MY_LOVE,
    SAND,
    WATER,
    BEDROCK
};

enum class BlockType {
    OPAQUE = 0,
    CUTOUT,
    TRANSPARENT
};

struct SideUV {
    glm::vec2 uv[4]; // 4 vertices per cube face
};

enum class BlockSide {
    TOP = 0,
    BOTTOM = 1,
    LEFT = 2,
    RIGHT = 3,
    FRONT = 4,
    BACK = 5
};

struct BlockTexture {
    SideUV sides[6]; // 6 cube faces
};

struct BlockInfo {
    std::string name = "";
    BlockType block_type;
    bool is_solid_surface = true;
    BlockTexture texture = {};
};

inline std::unordered_map<BlockID, BlockInfo> BlocksInfo = {
    {BlockID::EMPTY, {"", BlockType::TRANSPARENT}},
    {BlockID::ERROR, {"error", BlockType::OPAQUE}},
    {BlockID::STONE, {"stone", BlockType::OPAQUE}},
    {BlockID::DIRT, {"dirt", BlockType::OPAQUE}},
    {BlockID::GRASS, {"grass", BlockType::OPAQUE}},
    {BlockID::SNOW_GRASS, {"snow_grass", BlockType::OPAQUE}},
    {BlockID::SAND, {"sand", BlockType::OPAQUE}},
    {BlockID::WATER, {"water", BlockType::TRANSPARENT}},
    {BlockID::BEDROCK, {"bedrock", BlockType::OPAQUE}},
    {BlockID::MY_LOVE, {"love", BlockType::OPAQUE}},
};


class Block {
public:
    Block(BlockID block_id = BlockID::EMPTY);
    ~Block();

    BlockID getBlockID();
    void setBlockID(BlockID block);
private:
    BlockID m_block_id;
};