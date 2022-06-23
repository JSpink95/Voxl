#include "utility/file_io.h"

#include <fstream>
#include <sstream>

namespace platform::io
{
	std::string load_file_to_string(
		std::string_view filePath)
	{
		std::ifstream file(filePath.data());
		if (!file.is_open())
		{
			return "";
		}

		std::stringstream buffer;
		buffer << file.rdbuf();

		file.close();

		return buffer.str();
	}
}