#include "chunk.h"
#include "../core/logger.h"

#include <GLFW/glfw3.h>

Chunk::Chunk() {
	m_neighbors.x_p.store(nullptr);
	m_neighbors.z_p.store(nullptr);
	m_neighbors.x_m.store(nullptr);
	m_neighbors.z_m.store(nullptr);
	m_dirty = true;

	m_storage = std::make_unique<ChunkStorage>();
	m_mesh_renderer = std::make_unique<MeshRenderer>();

	m_ready_gpu.ready = false;
}

Chunk::~Chunk() {
}

void Chunk::markDirty() {
	std::lock_guard<std::mutex> lock(m_dirty_mutex);
	m_dirty = true;
}

Block* Chunk::getBlock(glm::ivec3 position) {
	if (position.y < 0 || position.y >= CHUNK_SIZE_Y) return nullptr;

	if (position.x < 0) {
		Chunk* neighbor = m_neighbors.x_m.load();
		if (neighbor == nullptr) return nullptr;
		return neighbor->getBlock({ CHUNK_SIZE_X + position.x, position.y, position.z });
	}
	if (position.z < 0) {
		Chunk* neighbor = m_neighbors.z_m.load();
		if (neighbor == nullptr) return nullptr;
		return neighbor->getBlock({ position.x, position.y, CHUNK_SIZE_Z + position.z });
	}
	if (position.x >= CHUNK_SIZE_X) {
		Chunk* neighbor = m_neighbors.x_p.load();
		if (neighbor == nullptr) return nullptr;
		return neighbor->getBlock({ position.x - CHUNK_SIZE_X, position.y, position.z });
	}
	if (position.z >= CHUNK_SIZE_Z) {
		Chunk* neighbor = m_neighbors.z_p.load();
		if (neighbor == nullptr) return nullptr;
		return neighbor->getBlock({ position.x, position.y, position.z - CHUNK_SIZE_Z });
	}

	return m_storage->getBlock(position);
}

Block* Chunk::getBlockLocal(glm::ivec3 position) {
	return m_storage->getBlock(position);
}

Block* Chunk::setBlock(glm::ivec3 position, BlockID block, bool mark) {
	if (!m_storage->setBlock(position, block)) return nullptr;
	if (mark) markDirty();
	return m_storage->getBlock(position);
}

void Chunk::calculateMesh() {
	if (!m_dirty) return;

	ChunkStorage* neighbor_x_p = nullptr;
	ChunkStorage* neighbor_z_p = nullptr;
	ChunkStorage* neighbor_x_m = nullptr;
	ChunkStorage* neighbor_z_m = nullptr;

	if (m_neighbors.x_p.load() != nullptr) neighbor_x_p = m_neighbors.x_p.load()->getStorage();
	if (m_neighbors.z_p.load() != nullptr) neighbor_z_p = m_neighbors.z_p.load()->getStorage();
	if (m_neighbors.x_m.load() != nullptr) neighbor_x_m = m_neighbors.x_m.load()->getStorage();
	if (m_neighbors.z_m.load() != nullptr) neighbor_z_m = m_neighbors.z_m.load()->getStorage();

	Mesh generated_mesh = m_mesh_generator.generateMesh(
		m_storage.get(),
		neighbor_x_p,
		neighbor_z_p,
		neighbor_x_m,
		neighbor_z_m
	);

	{
		std::lock_guard<std::mutex> lock(m_dirty_mutex);
		m_ready_gpu.mesh = std::move(generated_mesh);
		m_ready_gpu.ready = true;
		m_dirty = false;
	}
}

void Chunk::uploadMeshToGPU() {
	if (!m_ready_gpu.ready) return;
	if (m_ready_gpu.mesh.isEmpty()) return;

	m_mesh_renderer->uploadMesh(m_ready_gpu.mesh);

	m_ready_gpu.mesh.clear();
	m_ready_gpu.ready = false;
}

void Chunk::draw() {
	if (m_ready_gpu.ready) uploadMeshToGPU();
	m_mesh_renderer->draw();
}

void Chunk::updateNeighbors(Chunk* x_p, Chunk* z_p, Chunk* x_m, Chunk* z_m) {
	if (x_p != nullptr) m_neighbors.x_p.store(x_p);
	if (z_p != nullptr) m_neighbors.z_p.store(z_p);
	if (x_m != nullptr) m_neighbors.x_m.store(x_m);
	if (z_m != nullptr) m_neighbors.z_m.store(z_m);
}

bool Chunk::isDirty() {
	std::lock_guard<std::mutex> lock(m_dirty_mutex);
	return m_dirty;
}

int Chunk::getTopBlockPosition(int x, int z) {
	return m_storage->getTopBlockY(x, z);
}