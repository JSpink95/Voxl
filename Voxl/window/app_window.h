#pragma once

#include <core_types.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <array>
#include <functional>
#include <string_view>

struct input_context_t;

struct app_window_t
{
	SDL_Window* handle = nullptr;
	SDL_Renderer* renderer = nullptr;

	// opengl
	SDL_GLContext context;

	// input
	input_context_t* inputContext = nullptr;
};

void initialise_app_window(
	  app_window_t* window
	, std::string_view title
	, s32 width
	, s32 height);

void initialise_opengl(
	app_window_t* window);

void app_window_begin_frame(
	app_window_t* window);

bool app_window_poll_events(
	  app_window_t* window
	, std::function<void(SDL_Event)> handler);

void app_window_present(
	app_window_t* window);

