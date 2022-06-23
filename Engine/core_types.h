#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdint>
#include <string_view>
#include <format>
#include <Windows.h>

using s8 = std::int8_t;
using u8 = std::uint8_t;

using s16 = std::int16_t;
using u16 = std::uint16_t;

using s32 = std::int32_t;
using u32 = std::uint32_t;

using s64 = std::int64_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

#define SAFE_CLEANUP(thing, code) if (thing) { code delete thing; } thing = nullptr

namespace Console
{
	template<typename...TArgs>
	inline void Log(std::string_view format, TArgs&&...args)
	{
		auto formatted = std::vformat(format, std::make_format_args(std::forward<TArgs>(args)...));
		printf("%s\n", formatted.c_str());
		OutputDebugStringA(formatted.data());
	}

	template<typename...TArgs>
	inline void LogWarning(std::string_view format, TArgs&&...args)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);

		Log(format, std::forward<const TArgs>(args)...);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

	template<typename...TArgs>
	inline void LogError(std::string_view format, TArgs&&...args)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);

		Log(format, std::forward<TArgs>(args)...);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}