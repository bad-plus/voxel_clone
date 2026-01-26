#include "world_generator.h"
#include <core/world/block/block.h>
#include <core/world/chunk/chunk.h>
#include "biome.hpp"
#include <core/logger.hpp>
#include <core/constants.h>

void WorldGenerator::generateTerrain(Chunk* chunk, int x, int z) {
	for (int px = 0; px < Constants::CHUNK_SIZE_X; px++) {
		for (int pz = 0; pz < Constants::CHUNK_SIZE_Z; pz++) {
			int world_pos_x = (x * Constants::CHUNK_SIZE_X) + px;
			int world_pos_z = (z * Constants::CHUNK_SIZE_Z) + pz;

			auto weights = Biome::getBiomeWeight(m_perlin.get(), world_pos_x, world_pos_z);
			Biome::BiomeID biome = Biome::getDominantBiome(weights);

			int height = Biome::getHeight(m_biome_perlins, world_pos_x, world_pos_z, weights);

			for (int py = 0; py < height; py++) {
				BlockID block = BlockID::EMPTY;
				if (py == height - 1)
					block = Biome::BiomesInfo[biome].top_block;
				else if (py < height - 1 && py > height - 6)
					block = Biome::BiomesInfo[biome].under_block;
				else
					block = BlockID::STONE;

				chunk->setBlock({ px, py, pz }, block);
			}
		}
	}
}