#include <iostream>
#include <sstream>
#include <string_view>

#include <core_types.h>

u32 hexstring_to_int(std::string_view hex)
{
	std::stringstream stream;
	stream << std::hex << hex;

	u32 result = 0;
	stream >> result;

	return result;
}

glm::vec3 hex_to_rgb(u32 hex)
{
	u8 r = (hex >> 16) & 0xff;
	u8 g = (hex >> 8) & 0xff;
	u8 b = (hex >> 0) & 0xff;

	return (glm::vec3(r, g, b) / 255.0f);
}

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		Console::LogError("No command found.");
		return - 1;
	}

	std::string_view command = argv[1];
	if (command == "-hex_convert")
	{
		for (s32 argIndex = 2; argIndex < argc; ++argIndex)
		{
			std::string_view valueString = argv[argIndex];
			glm::vec3 rgb = hex_to_rgb(hexstring_to_int(valueString));
			Console::Log("{0},{1},{2}", rgb.x, rgb.y, rgb.z);
		}
	}
	else
	{
		Console::LogError("Invalid command \"{0}\"\n", command);
	}
}