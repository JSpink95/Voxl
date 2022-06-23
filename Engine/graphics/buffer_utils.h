#pragma once
#include "core_types.h"
#include "graphics/core_graphics.h"

namespace ogl
{
	struct buffer_t
	{
		u32 id = 0;
	};

	void initialise_buffer(
		  buffer_t* buffer
		, GLenum type
		, size_t bytes
		, const void* data);

	void cleanup_buffer(
		buffer_t* buffer);

	void bind_buffer(
		  buffer_t* buffer
		, GLenum type);

	void update_buffer_data(
		  buffer_t* buffer
		, GLenum type
		, size_t bytes
		, const void* data);

	void update_buffer_data(
		  buffer_t* buffer
		, GLenum type
		, size_t offset
		, size_t bytes
		, const void* data);
}
