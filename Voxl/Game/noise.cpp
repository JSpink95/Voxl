#include "Game/noise.h"

#include "FastNoiseLite/FastNoiseLite.h"

namespace vxl
{
	void initialise_noise(
		  noise_t* noise
		, s32 seed/* = 1337*/)
	{
		noise->source = new FastNoiseLite(seed);
		noise->source->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		noise->source->SetFractalType(FastNoiseLite::FractalType_Ridged);
		noise->source->SetFractalOctaves(5);
		noise->source->SetFractalLacunarity(1.05f);
		noise->source->SetFrequency(0.01f);
	}

	void cleanup_noise(
		noise_t* noise)
	{
		SAFE_CLEANUP(noise->source, );
	}

	f32 get_noise_at(
		  noise_t* noise
		, s32 x
		, s32 y)
	{
		f32 value = noise->source->GetNoise<f32>((f32)x, (f32)y) * 0.5f + 0.5f;
		return value * 64.0f;
	}

	f32 get_noise_at(
		  noise_t* noise
		, s32 x
		, s32 y
		, s32 z)
	{
		f32 value = noise->source->GetNoise<f32>((f32)x, (f32)y, (f32)z) * 0.5f + 0.5f;
		return value * 64.0f;
	}
}