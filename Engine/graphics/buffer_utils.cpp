#include "graphics/buffer_utils.h"

namespace ogl
{
	void initialise_buffer(
		  buffer_t* buffer
		, GLenum type
		, size_t bytes
		, const void* data)
	{
		glGenBuffers(1, &buffer->id);
		glBindBuffer(type, buffer->id);
		glBufferData(type, bytes, data, GL_STATIC_DRAW);
		glBindBuffer(type, 0);
	}

	void cleanup_buffer(
		buffer_t* buffer)
	{
		glDeleteBuffers(1, &buffer->id);
	}

	void bind_buffer(
		  buffer_t* buffer
		, GLenum type)
	{
		glBindBuffer(type, buffer ? buffer->id : 0);
	}

	void update_buffer_data(
		  buffer_t* buffer
		, GLenum type
		, size_t bytes
		, const void* data)
	{
		update_buffer_data(buffer, type, 0, bytes, data);
	}

	void update_buffer_data(
		  buffer_t* buffer
		, GLenum type
		, size_t offset
		, size_t bytes
		, const void* data)
	{
		glBufferSubData(type, offset, bytes, data);
	}
}