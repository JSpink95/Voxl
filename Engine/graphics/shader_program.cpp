#include "graphics/shader_program.h"

namespace gl
{
	GLuint create_shader(std::string_view source, GLenum type)
	{
		GLuint shader = glCreateShader(type);

		const char* csource = source.data();
		glShaderSource(shader, 1, &csource, nullptr);
		glCompileShader(shader);

		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			GLint logLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

			std::string log(logLength, '#');
			glGetShaderInfoLog(shader, logLength, nullptr, log.data());

			Console::LogError("{0}\n", log);
		}

		return shader;
	}

	void attach_shaders(u32 program, u32 shaderCount, GLuint* shaders)
	{
		for (u32 shaderIndex = 0; shaderIndex < shaderCount; ++shaderIndex)
		{
			glAttachShader(program, shaders[shaderIndex]);
		}
	}

	void cleanup_shaders(u32 program, u32 shaderCount, GLuint* shaders)
	{
		for (u32 shaderIndex = 0; shaderIndex < shaderCount; ++shaderIndex)
		{
			glDetachShader(program, shaders[shaderIndex]);
			glDeleteShader(shaders[shaderIndex]);
		}
	}

	void link_shader_program(u32 program)
	{
		glLinkProgram(program);

		GLint success = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &success);

		if (success == GL_FALSE)
		{
			GLint logLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

			std::string log(logLength, '#');
			glGetProgramInfoLog(program, logLength, nullptr, log.data());

			Console::LogError("{0}\n", log);
		}
	}
}

namespace ogl
{
	void load_shader_program(
		  shader_program_t* program
		, std::string_view vs
		, std::string_view fs)
	{
		if (program->id != 0)
		{
			glDeleteProgram(program->id);
			program->id = 0;
		}

		GLuint shaders[2] =
		{
			 gl::create_shader(vs, GL_VERTEX_SHADER),
			 gl::create_shader(fs, GL_FRAGMENT_SHADER),
		};

		program->id = glCreateProgram();
		gl::attach_shaders(program->id, 2, shaders);
		gl::link_shader_program(program->id);
		gl::cleanup_shaders(program->id, 2, shaders);
	}

	void load_shader_program(
		  shader_program_t* program
		, std::string_view vs
		, std::string_view gs
		, std::string_view fs)
	{
		if (program->id != 0)
		{
			glDeleteProgram(program->id);
			program->id = 0;
		}

		GLuint shaders[3] =
		{
			 gl::create_shader(vs, GL_VERTEX_SHADER),
			 gl::create_shader(gs, GL_GEOMETRY_SHADER),
			 gl::create_shader(fs, GL_FRAGMENT_SHADER),
		};

		program->id = glCreateProgram();
		gl::attach_shaders(program->id, 3, shaders);
		gl::link_shader_program(program->id);
		gl::cleanup_shaders(program->id, 3, shaders);
	}

	void bind_shader_program(
		shader_program_t* program)
	{
		glUseProgram(program ? program->id : 0);
	}

	s32 get_program_uniform(
		  shader_program_t* program
		, std::string_view name)
	{
		return glGetUniformLocation(program->id, name.data());
	}

	void load_program_uniforms(
		  shader_program_t* program
		, std::vector<program_uniform_description_t>* uniforms)
	{
		constexpr size_t MAX_UNIFORM_NAME_LENGTH = 32;

		s32 uniformCount = 0;
		glGetProgramiv(program->id, GL_ACTIVE_UNIFORMS, &uniformCount);

		GLchar uniformNameBuffer[MAX_UNIFORM_NAME_LENGTH];

		// the TRUE uniform location, if we have an array we need to offset the location by the number of elements in said array.
		s32 uniformIndex = 0;

		for (s32 activeUniformIndex = 0; activeUniformIndex < uniformCount; ++activeUniformIndex)
		{
			ZeroMemory(uniformNameBuffer, MAX_UNIFORM_NAME_LENGTH);

			program_uniform_description_t description = {};
			description.location = uniformIndex;

			GLsizei length = 0;
			glGetActiveUniform(program->id, (GLuint)activeUniformIndex, MAX_UNIFORM_NAME_LENGTH, &length, &description.count, &description.type, uniformNameBuffer);

			description.isArray = (description.count > 1);
			length = (description.isArray ? length - 3 : length);

			description.name.resize(length);
			std::copy(uniformNameBuffer, uniformNameBuffer + length, description.name.data());

			// move to the next uniform index, taking into account array sizes
			uniformIndex += description.count;

			uniforms->push_back(description);
		}
	}

	void upload_uniform(
		shader_program_t*
		, s32 location
		, const glm::vec3& value)
	{
		glUniform3f(location, value.x, value.y, value.z);
	}

	void upload_uniform(
		shader_program_t*
		, s32 location
		, u32 count
		, const glm::vec3* values)
	{
		glUniform3fv(location, count, glm::value_ptr(*values));
	}

	void upload_uniform(
		shader_program_t*
		, s32 location
		, const glm::vec4& value)
	{
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void upload_uniform(
		shader_program_t*
		, s32 location
		, u32 count
		, const glm::vec4* values)
	{
		glUniform4fv(location, count, glm::value_ptr(*values));
	}

	void upload_uniform(
		  shader_program_t* program
		, s32 location
		, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void upload_uniform(
		  shader_program_t* program
		, s32 location
		, u32 count
		, const glm::mat4* matrices)
	{
		glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(*matrices));
	}
}