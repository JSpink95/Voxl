#include "Game/chunk.h"
#include "Game/noise.h"
#include "Game/chunk_manager.h"

#include "window/app_window.h"
#include "window/input_manager.h"

#include <core_types.h>

#include <graphics/core_graphics.h>
#include <graphics/vertex_array.h>
#include <graphics/shader_program.h>
#include <graphics/material.h>

#include <utility/file_io.h>

#include <functional>

struct camera_t
{
	glm::vec3 eye = glm::vec3(0.0f);
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
};

struct player_controller_t
{
	void update(f32 deltaTime);

	camera_t* camera = nullptr;
	glm::vec2 movementInput = { };
	glm::vec2 cameraInput = { };
	f32 yaw = 0.0f;
	f32 pitch = 0.0f;
};

void player_controller_t::update(f32 deltaTime)
{
	if (camera)
	{
		const glm::vec3 forward = camera->direction;
		const glm::vec3 right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));

		camera->eye += (forward * movementInput.x) + (right * movementInput.y);

		yaw += cameraInput.x * deltaTime;
		pitch += cameraInput.y * deltaTime;
		pitch = glm::clamp(pitch, -glm::half_pi<f32>(), glm::half_pi<f32>());
		
		camera->direction.x = glm::cos(yaw) * glm::cos(pitch);
		camera->direction.y = glm::sin(pitch);
		camera->direction.z = glm::sin(yaw) * glm::cos(pitch);
	}
}

glm::mat4 create_camera_matrix(
	camera_t* camera)
{
	return glm::lookAt(camera->eye, camera->eye + camera->direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

struct input_command_movement_t
	: public input_command_t
{
	player_controller_t* controller = nullptr;

	void execute(const input_command_params_t& params) final
	{
		if (controller)
		{
			if (params.x.has_value())
			{
				controller->movementInput.x = params.x.value();
			}

			if (params.y.has_value())
			{
				controller->movementInput.y = params.y.value();
			}
		}
	}
};

struct input_command_camera_t
	: public input_command_t
{
	player_controller_t* controller = nullptr;

	void execute(const input_command_params_t& params) final
	{
		if (params.x.has_value())
		{
			controller->cameraInput.x = params.x.value();
		}

		if (params.y.has_value())
		{
			controller->cameraInput.y = params.y.value();
		}
	}
};

const glm::vec3 _TYOldNostalgiaColorPalette[16] =
{
	glm::vec3(0.682f, 0.756f, 0.607f),
	glm::vec3(0.407f, 0.588f, 0.674f),
	glm::vec3(0.388f, 0.415f, 0.454f),
	glm::vec3(0.254f, 0.262f, 0.352f),
	glm::vec3(0.098f, 0.105f, 0.164f),
	glm::vec3(0.603f, 0.576f, 0.717f),
	glm::vec3(0.478f, 0.384f, 0.376f),
	glm::vec3(0.305f, 0.301f, 0.258f),
	glm::vec3(0.427f, 0.552f, 0.305f),
	glm::vec3(0.254f, 0.415f, 0.364f),
	glm::vec3(0.207f, 0.243f, 0.176f),
	glm::vec3(0.854f, 0.701f, 0.309f),
	glm::vec3(0.694f, 0.525f, 0.321f),
	glm::vec3(0.521f, 0.317f, 0.223f),
	glm::vec3(0.427f, 0.215f, 0.215f),
	glm::vec3(0.227f, 0.180f, 0.223f),
};

const glm::vec4 _YellowAdriftColorPalette[16] =
{
	glm::vec4(0.000f, 0.000f, 0.000f, 1.0f), // black
	glm::vec4(0.054f, 0.066f, 0.196f, 1.0f), //
	glm::vec4(0.180f, 0.223f, 0.282f, 1.0f), //
	glm::vec4(0.305f, 0.345f, 0.356f, 1.0f), //
	glm::vec4(0.549f, 0.568f, 0.576f, 1.0f), //
	glm::vec4(0.843f, 0.874f, 0.886f, 1.0f), //
	glm::vec4(0.294f, 0.694f, 0.827f, 1.0f), //
	glm::vec4(0.282f, 0.525f, 0.745f, 1.0f), //
	glm::vec4(0.254f, 0.450f, 0.431f, 1.0f), //
	glm::vec4(0.278f, 0.874f, 0.435f, 1.0f), //
	glm::vec4(0.839f, 1.000f, 0.364f, 1.0f), //
	glm::vec4(0.992f, 1.000f, 0.803f, 1.0f), //
	glm::vec4(1.000f, 0.721f, 0.219f, 1.0f), //
	glm::vec4(1.000f, 0.498f, 0.219f, 1.0f), //
	glm::vec4(1.000f, 0.219f, 0.305f, 1.0f), //
	glm::vec4(0.549f, 0.117f, 0.305f, 1.0f), //
};

ogl::shader_program_t voxelShaderProgram;
gfx::material_t voxelMaterial;

void reload_voxel_material()
{
	std::string voxelShaderFiles[3] =
	{
		platform::io::load_file_to_string("shaders/voxel.vs"),
		platform::io::load_file_to_string("shaders/voxel.gs"),
		platform::io::load_file_to_string("shaders/voxel.fs"),
	};

	ogl::load_shader_program(&voxelShaderProgram, voxelShaderFiles[0], voxelShaderFiles[1], voxelShaderFiles[2]);

	if (voxelMaterial.program == nullptr)
	{
		gfx::initialise_material(&voxelMaterial, &voxelShaderProgram);
	}
	else
	{
		gfx::reload_material(&voxelMaterial);
	}

	if (auto* parameter = gfx::get_material_parameter_array<glm::vec3>(&voxelMaterial, "_ColorPalette"))
	{
		std::copy(_YellowAdriftColorPalette, _YellowAdriftColorPalette + parameter->value.size(), parameter->value.data());
	}
}

void default_event_handler(SDL_Event event)
{
	switch (event.type)
	{
		case SDL_KEYDOWN:
		{
			if (event.key.keysym.sym == SDLK_SPACE)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
		} break;

		case SDL_KEYUP:
		{
			if (event.key.keysym.sym == SDLK_SPACE)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			if (event.key.keysym.sym == SDLK_r)
			{
				reload_voxel_material();
			}
		} break;
	}

	if (event.type == SDL_KEYUP)
	{
		if (event.key.keysym.sym == SDLK_r)
		{
			reload_voxel_material();
		}
	}
}

int main()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	app_window_t window;
	initialise_app_window(&window, "Voxl", 1920, 1080);
	initialise_opengl(&window);

	input_context_t inputContext;
	window.inputContext = &inputContext;

	reload_voxel_material();

	camera_t camera;
	camera.eye = glm::vec3(48.0f);
	camera.direction = -glm::normalize(camera.eye);

	player_controller_t controller;
	controller.camera = &camera;

	input_command_movement_t movementInputHandler;
	movementInputHandler.controller = &controller;
	{
		input_register_action(&inputContext, SDLK_a, input_action_t::PRESSED, { {}, -1.0f }, &movementInputHandler);
		input_register_action(&inputContext, SDLK_a, input_action_t::RELEASED, { {}, 0.0f }, &movementInputHandler);

		input_register_action(&inputContext, SDLK_d, input_action_t::PRESSED, { {}, 1.0f }, &movementInputHandler);
		input_register_action(&inputContext, SDLK_d, input_action_t::RELEASED, { {}, 0.0f }, &movementInputHandler);

		input_register_action(&inputContext, SDLK_w, input_action_t::PRESSED, { 1.0f, {} }, &movementInputHandler);
		input_register_action(&inputContext, SDLK_w, input_action_t::RELEASED, { 0.0f, {} }, &movementInputHandler);

		input_register_action(&inputContext, SDLK_s, input_action_t::PRESSED, { -1.0f, {} }, &movementInputHandler);
		input_register_action(&inputContext, SDLK_s, input_action_t::RELEASED, { 0.0f, {} }, &movementInputHandler);

	}

	input_command_camera_t cameraInputHandler;
	cameraInputHandler.controller = &controller;
	{
		input_register_action(&inputContext, SDLK_LEFT, input_action_t::PRESSED, { -1.0f, {} }, &cameraInputHandler);
		input_register_action(&inputContext, SDLK_LEFT, input_action_t::RELEASED, { 0.0f, {} }, &cameraInputHandler);

		input_register_action(&inputContext, SDLK_RIGHT, input_action_t::PRESSED, { 1.0f, {} }, &cameraInputHandler);
		input_register_action(&inputContext, SDLK_RIGHT, input_action_t::RELEASED, { 0.0f, {} }, &cameraInputHandler);

		input_register_action(&inputContext, SDLK_UP, input_action_t::PRESSED, { {}, 1.0f }, &cameraInputHandler);
		input_register_action(&inputContext, SDLK_UP, input_action_t::RELEASED, { {}, 0.0f }, &cameraInputHandler);

		input_register_action(&inputContext, SDLK_DOWN, input_action_t::PRESSED, { {}, -1.0f }, &cameraInputHandler);
		input_register_action(&inputContext, SDLK_DOWN, input_action_t::RELEASED, { {}, 0.0f }, &cameraInputHandler);
	}

	vxl::noise_t noise;
	vxl::initialise_noise(&noise, 404);

	vxl::chunk_manager_t chunkManager;
	vxl::initialise_chunk_manager(&chunkManager, &noise);

	bool shouldClose = false;
	while (!shouldClose)
	{
		controller.update(1.0f / 60.0f);
		vxl::update_chunks(&chunkManager, camera.eye);

		{
			std::vector<vxl::chunk_t*> chunks = {};
			vxl::get_dirty_chunks(&chunkManager, &chunks);

			for (vxl::chunk_t* chunk : chunks)
				vxl::generate_chunk_mesh(chunk);
		}

		app_window_begin_frame(&window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspectiveFov(glm::radians(90.0f), 1920.0f, 1080.0f, 0.01f, 1000.0f);
		glm::mat4 view = create_camera_matrix(&camera);

		if (auto* parameter = gfx::get_material_parameter<glm::mat4>(&voxelMaterial, "_Transform"))
		{
			parameter->value = glm::mat4(1.0f);
		}

		if (auto* parameter = gfx::get_material_parameter<glm::mat4>(&voxelMaterial, "_ViewProjection"))
		{
			parameter->value = projection * view;
		}

		if (auto* parameter = gfx::get_material_parameter<glm::vec3>(&voxelMaterial, "_CameraLookDirection"))
		{
			parameter->value = glm::normalize(camera.direction);
		}

		{
			std::vector<vxl::chunk_t*> chunks = {};
			vxl::get_all_chunks(&chunkManager, &chunks);

			gfx::material_bind_guard_t guard(&voxelMaterial);

			for (auto* chunk : chunks)
			{
				if (chunk == nullptr || chunk->mesh == nullptr)
					continue;

				if (auto* parameter = gfx::get_material_parameter<glm::mat4>(&voxelMaterial, "_Transform"))
				{
					parameter->set_value(glm::translate(glm::mat4(1.0f), glm::vec3(chunk->offset)));
				}

				ogl::submit_draw(chunk->mesh, GL_POINTS, chunk->vertexCount);
			}
		}

		shouldClose = app_window_poll_events(&window, default_event_handler);
		app_window_present(&window);
	}

	vxl::cleanup_chunk_manager(&chunkManager);
	vxl::cleanup_noise(&noise);
	gfx::cleanup_material(&voxelMaterial);

	SDL_Quit();
	return 0;
}
