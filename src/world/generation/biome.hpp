#pragma once
#include "../block.h"
#include "perlin_2d.h"
#include <map>
#include <vector>
#include <cmath>
#include <memory>

namespace Biome {
    enum class BiomeID {
        PLANKS = 0,
        HILLS = 1,
        SHARP_PEAKS = 2,
        WINTER = 3,
        DESERT = 4
    };

    struct BiomeInfo {
        float base_height = 10.0f;

        // Continent
        float frequency_continent = 0.0f;
        float amplitude_continent = 0.0f;

        // Terrain
        float frequency_terrain = 0.0f;
        float amplitude_terrain = 0.0f;

        // Details
        float frequency_details = 0.0f;
        float amplitude_details = 0.0f;

        // Ridge
        float frequency_ridge = 0.0f;
        float amplitude_ridge = 0.0f;

        BlockID top_block = BlockID::ERROR;
        BlockID under_block = BlockID::ERROR;

        float rarity = 1.0f;
    };

    inline std::map<BiomeID, BiomeInfo> BiomesInfo = {
        {BiomeID::PLANKS, {
            .base_height = 70.0f,

            .frequency_continent = 0.001f,
            .amplitude_continent = 60.0f,

            .frequency_terrain = 0.004f,
            .amplitude_terrain = 15.0f,

            .frequency_details = 0.02f,
            .amplitude_details = 5.0f,

            .frequency_ridge = 0.01f,
            .amplitude_ridge = 2.0f,

            .top_block = BlockID::GRASS,
            .under_block = BlockID::DIRT,

            .rarity = 3.0f
        }},
        {BiomeID::HILLS, {
            .base_height = 80.0f,

            .frequency_continent = 0.004f,
            .amplitude_continent = 120.0f,

            .frequency_terrain = 0.01f,
            .amplitude_terrain = 50.0f,

            .frequency_details = 0.025f,
            .amplitude_details = 12.0f,

            .frequency_ridge = 0.015f,
            .amplitude_ridge = 6.0f,

            .top_block = BlockID::GRASS,
            .under_block = BlockID::DIRT,

            .rarity = 2.0f
        }},

        {BiomeID::SHARP_PEAKS, {
            .base_height = 90.0f,

            .frequency_continent = 0.0025f,
            .amplitude_continent = 200.0f,

            .frequency_terrain = 0.012f,
            .amplitude_terrain = 90.0f,

            .frequency_details = 0.04f,
            .amplitude_details = 30.0f,

            .frequency_ridge = 0.025f,
            .amplitude_ridge = 20.0f,

            .top_block = BlockID::GRASS,
            .under_block = BlockID::DIRT,

            .rarity = 0.5f
        }},
        {BiomeID::WINTER, {
            .base_height = 95.0f,

            .frequency_continent = 0.003f,
            .amplitude_continent = 160.0f,

            .frequency_terrain = 0.008f,
            .amplitude_terrain = 70.0f,

            .frequency_details = 0.03f,
            .amplitude_details = 18.0f,

            .frequency_ridge = 0.018f,
            .amplitude_ridge = 10.0f,

            .top_block = BlockID::SNOW_GRASS,
            .under_block = BlockID::DIRT,

            .rarity = 1.0f
        }},
        {BiomeID::DESERT, {
            .base_height = 68.0f,

            .frequency_continent = 0.006f,
            .amplitude_continent = 25.0f,

            .frequency_terrain = 0.015f,
            .amplitude_terrain = 15.0f,

            .frequency_details = 0.04f,
            .amplitude_details = 4.0f,

            .frequency_ridge = 0.008f,
            .amplitude_ridge = 8.0f,

            .top_block = BlockID::SAND,
            .under_block = BlockID::SAND,

            .rarity = 1.5f
        }}
    };


    inline float smoother(float x) {
        return x * x * x * (x * (x * 6 - 15) + 10);
    }

    inline BiomeID getDominantBiome(const std::map<BiomeID, float>& weights) {
        BiomeID dominant = BiomeID::PLANKS;
        float max_weight = -1.0f;

        for (const auto& w : weights) {
            if (w.second > max_weight) {
                dominant = w.first;
                max_weight = w.second;
            }
        }

        return dominant;
    }

    inline std::map<BiomeID, float> getBiomeWeight(Perlin2D* perlin, int world_x, int world_z) {
        std::map<BiomeID, float> weights;

        const float biome_scale = 0.0001f; 
        float noise = perlin->noise((float)(world_x * biome_scale), (float)(world_z * biome_scale) + 1.0f) * 0.5f + 0.5f;

        float total_rarity = 0.0f;
        for (const auto& b : BiomesInfo) {
            total_rarity += b.second.rarity;
        }

        std::vector<float> cumulative_rarity;
        float cumulative = 0.0f;
        for (const auto& b : BiomesInfo) {
            cumulative += b.second.rarity / total_rarity;
            cumulative_rarity.push_back(cumulative);
        }

        float sum = 0.0f;
        int i = 0;
        for (const auto& b : BiomesInfo) {
            float start = (i == 0) ? 0.0f : cumulative_rarity[i - 1];
            float end = cumulative_rarity[i];
            float center = (start + end) / 2.0f;
            float half = (end - start) / 2.0f;

            float blend_zone = 0.8f;

            float t = 1.0f - std::fabs(noise - center) / (half * (1.0f + blend_zone));
            if (t < 0.0f) t = 0.0f;

            float w = smoother(smoother(t));
            weights[b.first] = w;
            sum += w;
            i++;
        }

        if (sum > 0.0001f) {
            for (auto& w : weights) {
                w.second /= sum;
            }
        }
        else {
            for (auto& w : weights) {
                w.second = BiomesInfo[w.first].rarity / total_rarity;
            }
        }

        return weights;
    }

	inline int getHeight(const std::map<BiomeID, std::unique_ptr<Perlin2D>>& perlins,
		int world_x, int world_z, std::map<BiomeID, float>& weights) {
		float result_height = 0.0f;

		for (const auto& w : weights) {
			BiomeID id = w.first;
			float weight = w.second;

			if (weight < 0.001f) continue;

			const BiomeInfo& info = BiomesInfo[id];

			auto it = perlins.find(id);
			if (it == perlins.end()) continue;
			Perlin2D* perlin = it->second.get();

			float height = 0.0f;

			float noise_continent = (perlin->noise(
				(float)world_x * info.frequency_continent,
				(float)world_z * info.frequency_continent) + 1.0f) * 0.5f * info.amplitude_continent;

			height += noise_continent;

			float noise_terrain = (perlin->noise(
				(float)world_x * info.frequency_terrain,
				(float)world_z * info.frequency_terrain) + 1.0f) * 0.5f * info.amplitude_terrain;

			height += noise_terrain;

			float noise_details = (perlin->noise(
				(float)world_x * info.frequency_details,
				(float)world_z * info.frequency_details) + 1.0f) * 0.5f * info.amplitude_details;

			height += noise_details;

			float noise_ridge_raw = perlin->noise(
				(float)world_x * info.frequency_ridge,
				(float)world_z * info.frequency_ridge);

			float noise_ridge = (1.0f - std::fabs(noise_ridge_raw)) * info.amplitude_ridge;
			height += noise_ridge;

			float smooth_weight = smoother(smoother(smoother(weight)));

			result_height += (info.base_height + height) * smooth_weight;
		}
            
		return (int)result_height;
	}
}