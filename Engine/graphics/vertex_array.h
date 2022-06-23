#pragma once
#include "core_types.h"
#include "graphics/core_graphics.h"

namespace ogl
{
	struct buffer_t;
	struct vertex_array_t
	{
		u32 id = 0;
		buffer_t* indexBuffer = nullptr;
		buffer_t* vertexBuffer = nullptr;
	};

	void initialise_vertex_array(
		  vertex_array_t* vertexArray
		, size_t vertexBytes
		, const void* vertices);

	void initialise_vertex_array(
		  vertex_array_t* vertexArray
		, size_t vertexBytes
		, const void* vertices
		, size_t indexCount
		, const u16* indices);

	void cleanup_vertex_array(
		vertex_array_t* vertexArray);

	void enable_vertex_attribute(
		  vertex_array_t* vertexArray
		, u32 index
		, GLenum type
		, u32 stride
		, u32 elements
		, u32 offset);

	void update_index_buffer(
		  vertex_array_t* vertexArray
		, size_t count
		, const u16* data);

	void update_vertex_buffer(
		  vertex_array_t* vertexArray
		, size_t bytes
		, const void* data);

	void submit_draw(
		  vertex_array_t* vertexArray
		, GLenum mode
		, u32 count);
}
