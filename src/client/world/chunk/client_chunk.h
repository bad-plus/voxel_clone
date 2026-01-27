#pragma once
#include <core/world/chunk/chunk.h>

#include "mesh_generator.h"

#include <glad/glad.h>
#include <mutex>
#include <atomic>
#include <core/time.hpp>

#include "../../render/graphics/mesh.h"
#include "../../render/graphics/mesh_renderer.h"

class ClientChunk : public Chunk {
public:
	ClientChunk();
	~ClientChunk() override = default;

	void drawOpaque();
	void drawCutout();
	void drawTransparent();

	void markDirty();
	bool isDirty();

	void updateNeighbors(Chunk* x_p = nullptr, Chunk* z_p = nullptr, Chunk* x_m = nullptr, Chunk* z_m = nullptr);

	void calculateMesh();
	void uploadMeshToGPU();

	Time getChunkBuildTime();
private:
	struct {
		ChunkMesh mesh;
		std::atomic<bool> ready;
	} m_ready_gpu;

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

	Time m_mesh_build_time;
};