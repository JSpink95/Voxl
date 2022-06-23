#include "graphics/vertex_array.h"
#include "graphics/buffer_utils.h"

namespace ogl
{
	void initialise_vertex_array(
		  vertex_array_t* vertexArray
		, size_t vertexBytes
		, const void* vertices)
	{
		glCreateVertexArrays(1, &vertexArray->id);
		glBindVertexArray(vertexArray->id);

		vertexArray->vertexBuffer = new buffer_t{};
		initialise_buffer(vertexArray->vertexBuffer, GL_ARRAY_BUFFER, vertexBytes, vertices);

		glBindVertexArray(0);
	}

	void initialise_vertex_array(
		  vertex_array_t* vertexArray
		, size_t vertexBytes
		, const void* vertices
		, size_t indexCount
		, const u16* indices)
	{
		glCreateVertexArrays(1, &vertexArray->id);
		glBindVertexArray(vertexArray->id);

		vertexArray->vertexBuffer = new buffer_t{};
		initialise_buffer(vertexArray->vertexBuffer, GL_ARRAY_BUFFER, vertexBytes, vertices);

		vertexArray->indexBuffer = new buffer_t{};
		initialise_buffer(vertexArray->indexBuffer, GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(u16), indices);

		glBindVertexArray(0);
	}

	void cleanup_vertex_array(
		vertex_array_t* vertexArray)
	{
		SAFE_CLEANUP(vertexArray->vertexBuffer, cleanup_buffer(vertexArray->vertexBuffer););
		SAFE_CLEANUP(vertexArray->indexBuffer, cleanup_buffer(vertexArray->indexBuffer););
		glDeleteVertexArrays(1, &vertexArray->id);
	}

	void enable_vertex_attribute(
		  vertex_array_t* vertexArray
		, u32 index
		, GLenum type
		, u32 stride
		, u32 elements
		, u32 offset)
	{
		glBindVertexArray(vertexArray->id);

		glEnableVertexAttribArray(index);

		switch (type)
		{
			case GL_INT:
			case GL_UNSIGNED_INT:
				glVertexAttribIFormat(index, elements, type, offset);
				break;

			default:
				glVertexAttribFormat(index, elements, type, GL_FALSE, offset);
				break;
		}

		glVertexAttribBinding(index, 0);
		glBindVertexBuffer(index, vertexArray->vertexBuffer->id, 0u, (GLsizei)stride);

		glBindVertexArray(0);
	}

	void update_index_buffer(
		  vertex_array_t* vertexArray
		, size_t count
		, const u16* data)
	{
		glBindVertexArray(vertexArray->id);
		bind_buffer(vertexArray->indexBuffer, GL_ARRAY_BUFFER);
		update_buffer_data(vertexArray->vertexBuffer, GL_ARRAY_BUFFER, count * sizeof(u16), data);
		bind_buffer(nullptr, GL_ARRAY_BUFFER);
		glBindVertexArray(0);
	}

	void update_vertex_buffer(
		  vertex_array_t* vertexArray
		, size_t bytes
		, const void* data)
	{
		glBindVertexArray(vertexArray->id);
		bind_buffer(vertexArray->vertexBuffer, GL_ARRAY_BUFFER);
		update_buffer_data(vertexArray->vertexBuffer, GL_ARRAY_BUFFER, bytes, data);
		bind_buffer(nullptr, GL_ARRAY_BUFFER);
		glBindVertexArray(0);
	}

	void submit_draw(
		  vertex_array_t* vertexArray
		, GLenum mode
		, u32 count)
	{
		// nothing to draw nothing to see
		if (count == 0)
		{
			return;
		}

		glBindVertexArray(vertexArray->id);

		bind_buffer(vertexArray->vertexBuffer, GL_ARRAY_BUFFER);

		if (vertexArray->indexBuffer)
		{
			bind_buffer(vertexArray->indexBuffer, GL_ELEMENT_ARRAY_BUFFER);
			glDrawElements(mode, count, GL_UNSIGNED_SHORT, nullptr);
			bind_buffer(nullptr, GL_ELEMENT_ARRAY_BUFFER);
		}
		else
		{
			glDrawArrays(mode, 0, count);
		}

		bind_buffer(nullptr, GL_ARRAY_BUFFER);
		glBindVertexArray(0);
	}
}