#pragma once
#include "core_types.h"
#include "graphics/material_parameter.h"

#include <unordered_map>

namespace gfx
{
	struct material_t
	{
		ogl::shader_program_t* program = nullptr;
		std::unordered_map<std::string, material_parameter_base_t*> uniforms;
	};

	struct material_bind_guard_t
	{
		material_bind_guard_t(material_t* material);
		~material_bind_guard_t();

		material_t* material = nullptr;
	};

	void initialise_material(
		  material_t* material
		, ogl::shader_program_t* program);

	void cleanup_material(
		material_t* material);

	void reload_material(
		material_t* material);

	void bind_material_properties(
		material_t* material);

	void unbind_material_properties(
		material_t* material);

	material_parameter_base_t* get_material_parameter_generic(
		  material_t* material
		, std::string_view name);

	template<typename T>
	inline material_parameter_t<T>* get_material_parameter(
		  material_t* material
		, std::string_view name)
	{
		return dynamic_cast<material_parameter_t<T>*>(get_material_parameter_generic(material, name));
	}

	template<typename T>
	inline material_parameter_array_t<T>* get_material_parameter_array(
		  material_t* material
		, std::string_view name)
	{
		return dynamic_cast<material_parameter_array_t<T>*>(get_material_parameter_generic(material, name));
	}
}