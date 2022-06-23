#pragma once
#include "core_types.h"
#include "graphics/shader_program.h"

#include <any>

namespace gfx
{
	struct material_parameter_base_t
	{
		virtual ~material_parameter_base_t() = default;

		virtual void set_value(std::any newValue) = 0;
		virtual void upload_uniform(ogl::shader_program_t* program) = 0;

		s32 location = -1;
		ogl::shader_program_t* boundShader = nullptr;
	};

	template<typename T>
	struct material_parameter_t
		: public material_parameter_base_t
	{
		void set_value(std::any newValue) final;
		void upload_uniform(ogl::shader_program_t* program) final;

		T value;
	};

	template<typename T>
	struct material_parameter_array_t
		: public material_parameter_base_t
	{
		material_parameter_array_t(u64 size);

		void set_value(std::any newValue) final;
		void upload_uniform(ogl::shader_program_t* program) final;

		std::vector<T> value = {};
	};

	using material_parameter_vec3 = material_parameter_t<glm::vec3>;
	using material_parameter_mat4 = material_parameter_t<glm::mat4>;
}
#include "material_parameter.inl"