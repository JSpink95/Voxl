#pragma once
#include <core_types.h>

#include "window/app_window.h"

// stl
#include <optional>

struct input_command_params_t
{
	input_command_params_t() = default;
	input_command_params_t(const input_command_params_t& params)
		: x(params.x), y(params.y) {}
	input_command_params_t(std::optional<f32> inX, std::optional<f32> inY)
		: x(inX), y(inY) {}

	std::optional<f32> x = {};
	std::optional<f32> y = {};
};

struct input_command_t
{
	virtual ~input_command_t() = default;

	virtual void execute(const input_command_params_t& params) = 0;
};

struct input_action_t
{
	enum state_e { PRESSED, RELEASED, MAX };

	input_command_params_t keys = {};
	input_command_t* command = nullptr;
};

struct input_axis_t
{
	input_command_params_t axis = {};
	input_command_t* command = nullptr;
};

struct input_mapping_t
{
	std::array<std::vector<input_action_t>, input_action_t::MAX> actions = {};
	std::vector<input_axis_t> axes = {};
};

struct input_context_t
{
	std::unordered_map<SDL_Keycode, input_mapping_t> mappings = {};
};

void input_register_action(
	  input_context_t* context
	, SDL_Keycode key
	, input_action_t::state_e state
	, const input_command_params_t& params
	, input_command_t* command);

void input_register_axis(
	input_context_t* context);

void input_handle_key_event(
	  input_context_t* context
	, SDL_Keycode key
	, input_action_t::state_e state);
