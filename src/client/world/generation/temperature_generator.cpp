#include "world_generator.h"
#include <core/world/block/block.h>
#include "../chunk/chunk.h"
#include <core/logger.hpp>
#include "perlin_2d.h"
#include <cmath>

const float frenquency_temperature = 0.0001f;

TEMPERATURE_T WorldGenerator::getTemperature(int world_block_x, int world_block_z)
{
	float temperature_noise =
		(m_perlin_temperature->noise(
			world_block_x * frenquency_temperature,
			world_block_z * frenquency_temperature
		) + 1.0f) * 0.5f;

	int max_index = static_cast<int>(TEMPERATURE_T::END);
	int index = static_cast<int>(temperature_noise * max_index);
	return static_cast<TEMPERATURE_T>(index);
}