#include "client_chunk.h"

#include <GLFW/glfw3.h>

ClientChunk::ClientChunk()
{
	m_neighbors.x_p.store(nullptr);
	m_neighbors.z_p.store(nullptr);
	m_neighbors.x_m.store(nullptr);
	m_neighbors.z_m.store(nullptr);
	m_dirty = true;

	m_mesh_renderer_opaque = std::make_unique<MeshRenderer>();
	m_mesh_renderer_cutout = std::make_unique<MeshRenderer>();
	m_mesh_renderer_transparent = std::make_unique<MeshRenderer>();

	m_ready_gpu.ready = false;

	m_mesh_build_time = 0.0f;
}

ClientChunk::~ClientChunk()
{

}

void ClientChunk::markDirty() {
	std::lock_guard<std::mutex> lock(m_dirty_mutex);
	m_dirty = true;
}

void ClientChunk::calculateMesh() {
	if (!m_dirty) return;

	Time start_generation_time = Time::now();

	ChunkStorage* neighbor_x_p = nullptr;
	ChunkStorage* neighbor_z_p = nullptr;
	ChunkStorage* neighbor_x_m = nullptr;
	ChunkStorage* neighbor_z_m = nullptr;

	if (m_neighbors.x_p.load() != nullptr) neighbor_x_p = m_neighbors.x_p.load()->getStorage();
	if (m_neighbors.z_p.load() != nullptr) neighbor_z_p = m_neighbors.z_p.load()->getStorage();
	if (m_neighbors.x_m.load() != nullptr) neighbor_x_m = m_neighbors.x_m.load()->getStorage();
	if (m_neighbors.z_m.load() != nullptr) neighbor_z_m = m_neighbors.z_m.load()->getStorage();

	ChunkMesh generated_mesh = m_mesh_generator.generateMesh(
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

	m_mesh_build_time = Time::now() - start_generation_time;
}

void ClientChunk::uploadMeshToGPU() {
	if (!m_ready_gpu.ready) return;

	if (!m_ready_gpu.mesh.opaque.isEmpty()) {
		m_mesh_renderer_opaque->uploadMesh(m_ready_gpu.mesh.opaque);
	}
	if (!m_ready_gpu.mesh.cutout.isEmpty()) {
		m_mesh_renderer_cutout->uploadMesh(m_ready_gpu.mesh.cutout);
	}
	if (!m_ready_gpu.mesh.transparent.isEmpty()) {
		m_mesh_renderer_transparent->uploadMesh(m_ready_gpu.mesh.transparent);
	}

	m_ready_gpu.mesh.opaque.clear();
	m_ready_gpu.mesh.cutout.clear();
	m_ready_gpu.mesh.transparent.clear();
	m_ready_gpu.ready = false;
}

void ClientChunk::drawOpaque() {
	if (m_ready_gpu.ready) uploadMeshToGPU();
	m_mesh_renderer_opaque->draw();
}

void ClientChunk::drawCutout() {
	if (m_ready_gpu.ready) uploadMeshToGPU();
	m_mesh_renderer_cutout->draw();
}

void ClientChunk::drawTransparent() {
	if (m_ready_gpu.ready) uploadMeshToGPU();
	m_mesh_renderer_transparent->draw();
}

void ClientChunk::updateNeighbors(Chunk* x_p, Chunk* z_p, Chunk* x_m, Chunk* z_m) {
	if (x_p != nullptr) m_neighbors.x_p.store(x_p);
	if (z_p != nullptr) m_neighbors.z_p.store(z_p);
	if (x_m != nullptr) m_neighbors.x_m.store(x_m);
	if (z_m != nullptr) m_neighbors.z_m.store(z_m);
}

bool ClientChunk::isDirty() {
	std::lock_guard<std::mutex> lock(m_dirty_mutex);
	return m_dirty;
}

Time ClientChunk::getChunkBuildTime() {
	if (m_ready_gpu.ready) return Time();

	return m_mesh_build_time;
}