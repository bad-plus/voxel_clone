#pragma once
#include "block.h"

#include <unordered_map>

namespace Biome {
    enum class BiomeID {
        PLANKS = 0,
        HILLS,
        WINTER
    };

    struct BiomeInfo {
        float base_height;
        float height_variation;

        BlockID top_block;
        BlockID under_block;

        float min_noise;
        float max_noise;
    };

    inline std::unordered_map<BiomeID, BiomeInfo> BiomesInfo = {
        {BiomeID::PLANKS, {
            .base_height = 100,
            .height_variation = 40,
            
            .top_block = BlockID::GRASS,
            .under_block = BlockID::DIRT
        }},

        {BiomeID::HILLS, {
            .base_height = 300,
            .height_variation = 150,
            
            .top_block = BlockID::GRASS,
            .under_block = BlockID::DIRT
        }},

        {BiomeID::WINTER, {
            .base_height = 130,
            .height_variation = 50,
            
            .top_block = BlockID::SNOW_GRASS,
            .under_block = BlockID::DIRT
        }}
    };

    void autoAssingBiomeRanges() {
        int count = BiomesInfo.size();
        if(count == 0) return;

        float step = 1.0f / count;

        int i = 0;
        for(auto& kv : BiomesInfo) {
            float min_value = step * i;
            float max_value = step * (i + 1);

            kv.second.min_noise = min_value;
            kv.second.max_noise = max_value;

            i++;
        }
    }

    inline float smoother(float x) {
        return x * x * x * (x * (x * 6 - 15) + 10);
    }

    std::unordered_map<BiomeID, float> getBiomeWeight(Perlin2D* perlin, int world_x, int world_z) {
        std::unordered_map<BiomeID, float> weights;
        
        const float biome_scale = 0.003f;
        float noise = perlin->noise((float)(world_x * biome_scale), (float)(world_z * biome_scale) + 1.0f) * 0.5f;

        int n = BiomesInfo.size();
        float step = 1.0f / n;

        float sum = 0.0f;
        int i = 0;
        for(auto &b : BiomesInfo) {
            float center = step * (i * 0.5f);
            float half = step * 0.5f;

            float t = 1.0f - std::fabs(noise - center) / half;
            if(t < 0.0f) t = 0.0f;

            float w = smoother(t);
            weights[b.first] = w;
            sum += w;
            i++;
        }

        if(sum > 0.0001f) {
            for(auto &b : weights) {
                b.second /= sum;
            }
        }

        return weights;
    }

    BiomeID getDominantBiome(const std::unordered_map<BiomeID, float>& weights) {
        BiomeID dominant = BiomeID::PLANKS;
        float max_weight = -1.0f;

        for(auto &w : weights) {
            if(w.second > max_weight) {
                dominant = w.first;
                max_weight = w.second;
            }
        }

        return dominant;
    }

    int getHeight(Perlin2D* perlin, int world_x, int world_z, std::unordered_map<BiomeID, float>& weights) {
        float result_height = 0.0f;

        const float terrain_scale = 0.02f;

        for(auto &w : weights) {
            Biome::BiomeID id = w.first;
            float weight = w.second;
            
            float offset = (float)((int)id) * 50.0f;
            
            float noise= perlin->noise(
                (float)world_x * terrain_scale + offset, 
                (float)world_z * terrain_scale + offset);

            float height = BiomesInfo[id].base_height + noise * BiomesInfo[id].height_variation;

            result_height += height * weight;
        }

        return result_height;
    }
}