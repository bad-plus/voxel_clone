#pragma once

#include "block.h"
#include "chunk_storage.h"
#include "mesh_generator.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <mutex>
#include <atomic>
#include <memory>

#include "../render/graphics/mesh.h"
#include "../render/graphics/mesh_renderer.h"

struct Chunk;

class Chunk {
public:
	Chunk();
	~Chunk();

	int getTopBlockPosition(int x, int z);

	Block* getBlock(glm::ivec3 position);
	Block* setBlock(glm::ivec3 position, BlockID block, bool mark = false);

	void drawOpaque();
	void drawCutout();
	void drawTransparent();

	void markDirty();
	bool isDirty();

	void updateNeighbors(Chunk* x_p = nullptr, Chunk* z_p = nullptr, Chunk* x_m = nullptr, Chunk* z_m = nullptr);

	void calculateMesh();
	void uploadMeshToGPU();

	ChunkStorage* getStorage() { return m_storage.get(); }

	double getChunkBuildTime();
private:
	Block* getBlockLocal(glm::ivec3 position);

	struct {
		ChunkMesh mesh;
		std::atomic<bool> ready;
	} m_ready_gpu;

	std::unique_ptr<ChunkStorage> m_storage;

	std::unique_ptr<MeshRenderer> m_mesh_renderer_opaque;
	std::unique_ptr<MeshRenderer> m_mesh_renderer_cutout;
	std::unique_ptr<MeshRenderer> m_mesh_renderer_transparent;

	MeshGenerator m_mesh_generator;

	bool m_dirty;
	std::mutex m_dirty_mutex;

	struct {
		std::atomic<Chunk*> x_p;
		std::atomic<Chunk*> z_p;
		std::atomic<Chunk*> x_m;
		std::atomic<Chunk*> z_m;
	} m_neighbors;

	double m_mesh_build_time;
};