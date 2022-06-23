#pragma once

#include "core_types.h"

namespace vxl
{
	/*
		voxel data:
			xyz = 12 bits
			color = 4 bits, 16 color lookup table
			neighbours = 6 bits, 111111 <- neighbour active/inactive
			active = 1 bit
		total size: 4 bytes (32 bit)
		used bits: 23
		remaining bits: 9
	*/
	struct voxel_t
	{
		union
		{
			u32 data = 0;
			struct
			{
				u8 x : 4;			//  0-3		(4 bits)	positional data
				u8 y : 4;			//  4-7		(4 bits)		"
				u8 z : 4;			//  8-11	(4 bits)		"
				u8 c : 4;			// 12-15	(4 bits)	color data (lookup index)
				u8 neighbours : 6;	// 16-21	(6 bits)	neighbour data (who is active)
				u8 active : 1;		// 22-23	(1 bits)	is this voxel active
			};
		};
	};

	constexpr size_t voxel_byte_size = sizeof(voxel_t);

	voxel_t make_voxel(
		  u8 x
		, u8 y
		, u8 z
		, u8 c
		, bool active);

	void initialise_voxel(
		  voxel_t* voxel
		, u8 x
		, u8 y
		, u8 z
		, u8 c);
}
