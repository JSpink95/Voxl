#include "Game/voxel.h"

namespace vxl
{
	voxel_t make_voxel(
		  u8 x
		, u8 y
		, u8 z
		, u8 c
		, bool active)
	{
		voxel_t voxel;
		initialise_voxel(&voxel, x, y, z, c);

		return voxel;
	}

	void initialise_voxel(
		  voxel_t* voxel
		, u8 x
		, u8 y
		, u8 z
		, u8 c)
	{
		voxel->x = x;
		voxel->y = y;
		voxel->z = z;
		voxel->c = c;
	}
}