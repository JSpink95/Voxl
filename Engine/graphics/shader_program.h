#pragma once

#include "core_types.h"
#include "graphics/core_graphics.h"
#include <string_view>
#include <unordered_map>

namespace ogl
{
	struct shader_program_t
	{
		u32 id = 0u;
	};

	struct program_uniform_description_t
	{
		std::string name = "";
		GLenum type = GL_NONE;
		s32 location = -1;
		bool isArray = false;
		s32 count = 0;
	};

	// create and load a shader given vertex and fragment source
	void load_shader_program(
		  shader_program_t* program
		, std::string_view vs
		, std::string_view fs);

	// create and load a shader given vertex, geometry and fragment source
	void load_shader_program(
		  shader_program_t* program
		, std::string_view vs
		, std::string_view gs
		, std::string_view fs);

	// bind (or unbind) a shader to the pipeline
	void bind_shader_program(
		shader_program_t* program);
	
	// get a specific uniform location from the given shader
	s32 get_program_uniform(
		  shader_program_t* program
		, std::string_view name);

	void load_program_uniforms(
		  shader_program_t* program
		, std::vector<program_uniform_description_t>* uniforms);

	void upload_uniform(
		shader_program_t*
		, s32 location
		, const glm::vec3& value);

	void upload_uniform(
		  shader_program_t*
		, s32 location
		, u32 count
		, const glm::vec3* values);

	void upload_uniform(
		shader_program_t*
		, s32 location
		, const glm::vec4& value);

	void upload_uniform(
		shader_program_t*
		, s32 location
		, u32 count
		, const glm::vec4* values);

	void upload_uniform(
		  shader_program_t* program
		, s32 location
		, const glm::mat4& matrix);

	void upload_uniform(
		  shader_program_t* program
		, s32 location
		, u32 count
		, const glm::mat4* matrices);
}
