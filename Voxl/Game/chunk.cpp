#include "Game/chunk.h"
#include "Game/voxel.h"
#include "Game/noise.h"

#include <graphics/vertex_array.h>

#include <random>

namespace vxl
{
	constexpr u32 total_chunk_size = chunk_size * chunk_size * chunk_size;
	constexpr u8 all_neighbours_active = 0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20;

	namespace helper
	{
		u32 hash_chunk_coord(
			  s32 x
			, s32 y
			, s32 z)
		{
			return (x * chunk_size * chunk_size) + (y * chunk_size) + z;
		}

		bool is_voxel_active(
			  chunk_t* chunk
			, u32 index)
		{
			if (index >= total_chunk_size)
				return false;

			return chunk->voxels[index].active;
		}

		bool is_voxel_active(
			chunk_t* chunk
			, s32 x
			, s32 y
			, s32 z)
		{
			if (x < 0 || y < 0 || z < 0)
				return false;
			if (x >= chunk_size || y >= chunk_size || z >= chunk_size)
				return false;

			return is_voxel_active(chunk, hash_chunk_coord(x, y, z));
		}
	}

	void initialise_chunk(
		  chunk_t* chunk
		, noise_t* noise)
	{
		chunk->noise = noise;
		chunk->voxels = new voxel_t[total_chunk_size];
		for (u8 x = 0; x < (u8)(chunk_size); ++x)
		{
			for (u8 y = 0; y < (u8)(chunk_size); ++y)
			{
				for (u8 z = 0; z < (u8)(chunk_size); ++z)
				{
					voxel_t* voxel = get_voxel(chunk, x, y, z);
					initialise_voxel(voxel, x, y, z, 0);
				}
			}
		}
	
		chunk->mesh = new ogl::vertex_array_t{};
		ogl::initialise_vertex_array(chunk->mesh, total_chunk_size * sizeof(voxel_t), nullptr);
		ogl::enable_vertex_attribute(chunk->mesh, 0, GL_UNSIGNED_INT, sizeof(voxel_t), 1, 0);
	}

	void cleanup_chunk(
		chunk_t* chunk)
	{
		ogl::cleanup_vertex_array(chunk->mesh);
		SAFE_CLEANUP(chunk->mesh, );
		SAFE_CLEANUP(chunk->voxels, );
	}

	voxel_t* get_voxel(
		  chunk_t* chunk
		, u32 index)
	{
		return &chunk->voxels[index];
	}

	voxel_t* get_voxel(
		  chunk_t* chunk
		, u32 x
		, u32 y
		, u32 z)
	{
		const u32 index = helper::hash_chunk_coord(x, y, z);
		return get_voxel(chunk, index);
	}

	void generate_chunk_data(
		chunk_t* chunk)
	{
		std::default_random_engine random;
		const std::uniform_int_distribution<u16> colorDistribution(0, 15);

		random.seed(static_cast<u32>(std::time(0)));

		chunk->generatingMeshData = true;

		// first pass, generate the general mesh
		for (s32 index = 0; index < total_chunk_size; ++index)
		{
			voxel_t* voxel = get_voxel(chunk, index);
			voxel->c = static_cast<u8>(colorDistribution(random));

			const f32 noiseValue = get_noise_at(chunk->noise, chunk->offset.x + voxel->x, chunk->offset.z + voxel->z);
			voxel->active = static_cast<f32>(chunk->offset.y + voxel->y) < noiseValue;
		}

		// second pass, generate neighbour data
		for (s32 index = 0; index < total_chunk_size; ++index)
		{
			voxel_t* voxel = get_voxel(chunk, index);

			u8 neighbours = 0x0;

			if (helper::is_voxel_active(chunk, voxel->x - 1, voxel->y + 0, voxel->z + 0))
			{
				neighbours |= 0x1;
			}

			if (helper::is_voxel_active(chunk, voxel->x + 1, voxel->y + 0, voxel->z + 0))
			{
				neighbours |= 0x2;
			}

			if (helper::is_voxel_active(chunk, voxel->x + 0, voxel->y - 1, voxel->z + 0))
			{
				neighbours |= 0x4;
			}

			if (helper::is_voxel_active(chunk, voxel->x + 0, voxel->y + 1, voxel->z + 0))
			{
				neighbours |= 0x8;
			}

			if (helper::is_voxel_active(chunk, voxel->x + 0, voxel->y + 0, voxel->z - 1))
			{
				neighbours |= 0x10;
			}

			if (helper::is_voxel_active(chunk, voxel->x + 0, voxel->y + 0, voxel->z + 1))
			{
				neighbours |= 0x20;
			}

			voxel->neighbours = neighbours;
		}

		chunk->generatingMeshData = false;
		chunk->dirty = true;
	}

	void generate_chunk_mesh(
		chunk_t* chunk)
	{
		chunk->vertexCount = 0u;
		std::vector<voxel_t> voxels = {};
		for (s32 index = 0; index < total_chunk_size; ++index)
		{
			voxel_t* voxel = get_voxel(chunk, index);
		
			// only add to vertex list if active and not fully encapsulated
			if (voxel->active && voxel->neighbours != all_neighbours_active)
			{
				chunk->vertexCount += 1;
				voxels.push_back(*voxel);
			}
		}

		ogl::update_vertex_buffer(chunk->mesh, chunk->vertexCount * sizeof(voxel_t), voxels.data());
		chunk->dirty = false;
	}
}