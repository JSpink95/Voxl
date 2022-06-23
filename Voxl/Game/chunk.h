#pragma once
#include <core_types.h>
#include <atomic>

namespace ogl
{
	struct vertex_array_t;
}

namespace vxl
{
	struct voxel_t;
	struct noise_t;
	
	constexpr u32 chunk_size = 16;

	/*
		chunk data:
			voxels[16,16,16] = 16384 bytes
		total size: 16384 bytes (16 kilobytes)
	*/
	struct chunk_t
	{
		noise_t* noise = nullptr;
		voxel_t *voxels = nullptr;

		glm::ivec3 offset = glm::ivec3(0);

		u32 vertexCount = 0;
		ogl::vertex_array_t* mesh = nullptr;

		bool generatingMeshData = false;
		bool dirty = false;
	};

	void initialise_chunk(
		  chunk_t* chunk
		, noise_t* noise);

	void cleanup_chunk(
		chunk_t* chunk);

	auto get_voxel(
		chunk_t* chunk
		, u32 index) -> voxel_t*;

	auto get_voxel(
		chunk_t* chunk
		, u32 x
		, u32 y
		, u32 z) -> voxel_t*;

	// you should call this in a worker thread, no vertex processing happens here
	void generate_chunk_data(
		chunk_t* chunk);

	void generate_chunk_mesh(
		chunk_t* chunk);
}