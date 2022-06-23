#include "graphics/material.h"
#include "graphics/shader_program.h"

#include "core_graphics.h"

namespace gfx
{
	material_bind_guard_t::material_bind_guard_t(material_t* material)
	{
		this->material = material;
		bind_material_properties(material);
	}

	material_bind_guard_t::~material_bind_guard_t()
	{
		unbind_material_properties(material);
		material = nullptr;
	}

	void populate_material_parameters(
		  material_t* material
		, const std::vector<ogl::program_uniform_description_t>* descriptions)
	{
		for (const ogl::program_uniform_description_t& description : *descriptions)
		{
			switch (description.type)
			{
				case GL_FLOAT_VEC3:
					if (description.isArray)
					{
						material->uniforms.emplace(description.name, create_material_parameter_array<glm::vec3>(description.location, description.count));
					}
					else
					{
						material->uniforms.emplace(description.name, create_material_parameter<glm::vec3>(description.location));
					}
					break;

				case GL_FLOAT_VEC4:
					if (description.isArray)
					{
						material->uniforms.emplace(description.name, create_material_parameter_array<glm::vec4>(description.location, description.count));
					}
					else
					{
						material->uniforms.emplace(description.name, create_material_parameter<glm::vec4>(description.location));
					}
					break;

				case GL_FLOAT_MAT4:
					if (description.isArray)
					{
						material->uniforms.emplace(description.name, create_material_parameter_array<glm::mat4>(description.location, description.count));
					}
					else
					{
						material->uniforms.emplace(description.name, create_material_parameter<glm::mat4>(description.location));
					}
					break;

				default:
					Console::LogWarning("WARNING! Unhandled material parameter type {0}!", description.type);
					break;
			}
		}
	}

	void initialise_material(
		  material_t* material
		, ogl::shader_program_t* program)
	{
		material->program = program;

		reload_material(material);
	}

	void cleanup_material(
		material_t* material)
	{
		material->program = nullptr;

		for (auto [name, parameter] : material->uniforms)
		{
			SAFE_CLEANUP(parameter, );
		}
		material->uniforms.clear();
	}

	void reload_material(
		material_t* material)
	{
		for (auto [name, parameter] : material->uniforms)
		{
			SAFE_CLEANUP(parameter, );
		}
		material->uniforms.clear();

		std::vector<ogl::program_uniform_description_t> descriptions = {};
		ogl::load_program_uniforms(material->program, &descriptions);
		populate_material_parameters(material, &descriptions);
	}

	void bind_material_properties(
		material_t* material)
	{
		ogl::bind_shader_program(material->program);

		for (auto [name, parameter] : material->uniforms)
		{
			parameter->upload_uniform(material->program);
			parameter->boundShader = material->program;
		}
	}

	void unbind_material_properties(
		material_t* material)
	{
		ogl::bind_shader_program(nullptr);

		for (auto [name, parameter] : material->uniforms)
		{
			parameter->boundShader = nullptr;
		}
	}

	material_parameter_base_t* get_material_parameter_generic(
		  material_t* material
		, std::string_view name)
	{
		if (auto it = material->uniforms.find(name.data()); it != material->uniforms.end())
		{
			return it->second;
		}

		return nullptr;
	}
}