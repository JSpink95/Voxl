namespace gfx
{
	template<typename T>
	inline void material_parameter_t<T>::set_value(std::any newValue)
	{
		value = std::any_cast<T>(newValue);
		if (boundShader)
			upload_uniform(boundShader);
	}

	template<typename T>
	inline void material_parameter_t<T>::upload_uniform(ogl::shader_program_t* program)
	{
		ogl::upload_uniform(program, location, value);
	}

	template<typename T>
	material_parameter_array_t<T>::material_parameter_array_t(u64 size)
	{
		value.resize(size);
	}

	template<typename T>
	inline void material_parameter_array_t<T>::set_value(std::any newValue)
	{
		value = std::any_cast<std::vector<T>>(newValue);
		if (boundShader)
			upload_uniform(boundShader);
	}

	template<typename T>
	inline void material_parameter_array_t<T>::upload_uniform(ogl::shader_program_t* program)
	{
		ogl::upload_uniform(program, location, static_cast<u32>(value.size()), value.data());
	}

	template<typename T>
	inline material_parameter_t<T>* create_material_parameter(s32 location)
	{
		auto parameter = new material_parameter_t<T>();
		parameter->location = location;

		return parameter;
	}

	// TODO: need a dynamic array
	template<typename T>
	inline material_parameter_array_t<T>* create_material_parameter_array(s32 location, u64 count)
	{
		auto parameter = new material_parameter_array_t<T>(count);
		parameter->location = location;

		return parameter;
	}
}