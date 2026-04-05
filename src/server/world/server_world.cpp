#include "server_world.h"
#include "generation/world_generator.h"

ServerWorld::ServerWorld(WorldGenerator* generator) :
	m_generator(generator)
{
	if (m_generator == nullptr) {
		m_generator = new WorldGenerator(666);
	}

}

ServerWorld::~ServerWorld()
{

}

void ServerWorld::generateChunk(int x, int z)
{
	Chunk* chunk = getChunk(x, z);
	if (chunk == nullptr) chunk = createChunk(x, z);
	m_generator->generateChunk(chunk, x, z);
}
