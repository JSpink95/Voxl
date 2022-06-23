#pragma once
#include <core_types.h>

class FastNoiseLite;

namespace vxl
{
	struct noise_t
	{
		FastNoiseLite* source = nullptr;
	};

	void initialise_noise(
		  noise_t* noise
		, s32 seed = 1337);

	void cleanup_noise(
		noise_t* noise);

	f32 get_noise_at(
		  noise_t* noise
		, s32 x
		, s32 y);

	f32 get_noise_at(
		  noise_t* noise
		, s32 x
		, s32 y
		, s32 z);
}
