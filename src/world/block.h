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

struct SideUV {
    glm::vec2 uv[4]; // 4 vertecs per cube face
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
    bool isTransparent = false;
    BlockTexture texture = {};
};

inline std::unordered_map<BlockID, BlockInfo> BlocksInfo = {
    {BlockID::EMPTY, {"", true}},
    {BlockID::ERROR, {"error", false}},
    {BlockID::STONE, {"stone", false}},
    {BlockID::DIRT, {"dirt", false}},
    {BlockID::GRASS, {"grass", false}},
    {BlockID::SNOW_GRASS, {"snow_grass", false}},
    {BlockID::SAND, {"sand", false}},
    {BlockID::WATER, {"water", true}},
    {BlockID::BEDROCK, {"bedrock", false}},
    {BlockID::MY_LOVE, {"love", false}},
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