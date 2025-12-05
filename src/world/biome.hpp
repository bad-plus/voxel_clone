#pragma once
#include "block.h"
#include "perlin_2d.h"
#include <unordered_map>
#include <cmath> 

namespace Biome {
    enum class BiomeID {
        PLANKS = 0,
        HILLS,
        WINTER
    };

    struct BiomeInfo {
        float base_height;
        float height_variation;

        float terrain_scale;
        float terrain_offset;

        BlockID top_block;
        BlockID under_block;
    };

    inline std::unordered_map<BiomeID, BiomeInfo> BiomesInfo = {
        {BiomeID::PLANKS, {
            .base_height = 100.0f,
            .height_variation = 15.0f,

            .terrain_scale = 0.01f,
            .terrain_offset = 100.0f,

            .top_block = BlockID::GRASS,
            .under_block = BlockID::DIRT
        }},

        {BiomeID::HILLS, {
            .base_height = 200.0f,
            .height_variation = 30.0f,

            .terrain_scale = 0.008f,
            .terrain_offset = 500.0f,

            .top_block = BlockID::GRASS,
            .under_block = BlockID::DIRT
        }},

        {BiomeID::WINTER, {
            .base_height = 130.0f,
            .height_variation = 20.0f,

            .terrain_scale = 0.015f,
            .terrain_offset = 900.0f,

            .top_block = BlockID::SNOW_GRASS,
            .under_block = BlockID::DIRT
        }}
    };


    inline float smoother(float x) {
        return x * x * x * (x * (x * 6 - 15) + 10);
    }

    std::unordered_map<BiomeID, float> getBiomeWeight(Perlin2D* perlin, int world_x, int world_z) {
        std::unordered_map<BiomeID, float> weights;

        //const float biome_scale = 0.0001f;
        const float biome_scale = 0.001f;
        float noise = perlin->noise((float)(world_x * biome_scale), (float)(world_z * biome_scale) + 1.0f) * 0.5f + 0.5f;

        int n = BiomesInfo.size();
        float step = 1.0f / n;

        float sum = 0.0f;
        int i = 0;
        for (auto& b : BiomesInfo) {
            float center = step * (i + 0.5f);
            float half = step * 0.5f;

            float t = 1.0f - std::fabs(noise - center) / half;
            if (t < 0.0f) t = 0.0f;

            float w = smoother(t);
            weights[b.first] = w;
            sum += w;
            i++;
        }

        if (sum > 0.0001f) {
            for (auto& b : weights) {
                b.second /= sum;
            }
        }

        return weights;
    }

    BiomeID getDominantBiome(const std::unordered_map<BiomeID, float>& weights) {
        BiomeID dominant = BiomeID::PLANKS;
        float max_weight = -1.0f;

        for (auto& w : weights) {
            if (w.second > max_weight) {
                dominant = w.first;
                max_weight = w.second;
            }
        }

        return dominant;
    }

    int getHeight(Perlin2D* perlin, int world_x, int world_z, std::unordered_map<BiomeID, float>& weights) {
        float result_height = 0.0f;

        for (auto& w : weights) {
            Biome::BiomeID id = w.first;
            float weight = w.second;

            const BiomeInfo& info = BiomesInfo[id];

            float noise = perlin->noise(
                (float)world_x * info.terrain_scale + info.terrain_offset,
                (float)world_z * info.terrain_scale + info.terrain_offset);

            float height = info.base_height + noise * info.height_variation;

            result_height += height * weight;
        }

        return (int)result_height;
    }
}