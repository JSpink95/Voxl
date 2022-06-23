#include "window/app_window.h"
#include "window/input_manager.h"

#include <graphics/core_graphics.h>

void initialise_app_window(
	  app_window_t* window
	, std::string_view title
	, s32 width
	, s32 height)
{
	window->handle = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	window->renderer = SDL_CreateRenderer(window->handle, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
}

void initialise_opengl(
	app_window_t* window)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	window->context = SDL_GL_CreateContext(window->handle);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		Console::LogError("Failed to initialise GLEW!");
	}

	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}

void app_window_begin_frame(
	app_window_t* window)
{
	SDL_RenderClear(window->renderer);
}

bool app_window_poll_events(
	  app_window_t* window
	, std::function<void(SDL_Event)> handler)
{
	bool wantsExit = false;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			wantsExit = true;

		switch (event.type)
		{
			case SDL_WINDOWEVENT:
			{
				switch (event.window.type)
				{
					case SDL_WINDOWEVENT_CLOSE:
						wantsExit = true;
						break;
				}
			} break;

			case SDL_KEYDOWN:
			{
				if (!event.key.repeat)
				{
					input_handle_key_event(window->inputContext, event.key.keysym.sym, input_action_t::PRESSED);
				}
			} break;
			
			case SDL_KEYUP:
			{
				input_handle_key_event(window->inputContext, event.key.keysym.sym, input_action_t::RELEASED);
			} break;
		}

		if (handler)
			std::invoke(handler, event);
	}

	return wantsExit;
}

void app_window_present(
	app_window_t* window)
{
	SDL_GL_SwapWindow(window->handle);
}
