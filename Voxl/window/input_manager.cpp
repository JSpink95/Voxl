#include "window/input_manager.h"

void input_register_action(
	  input_context_t* context
	, SDL_Keycode key
	, input_action_t::state_e state
	, const input_command_params_t& params
	, input_command_t* command)
{
	if (auto it = context->mappings.find(key); it == context->mappings.end())
	{
		input_mapping_t mapping = {};
		context->mappings.emplace(key, mapping);
	}

	input_mapping_t& mapping = context->mappings.at(key);
	mapping.actions.at(state).push_back(
	{
		.keys = params,
		.command = command
	});
}

void input_register_axis(
	input_context_t* context)
{

}

void input_handle_key_event(
	  input_context_t* context
	, SDL_Keycode key
	, input_action_t::state_e state)
{
	if (auto it = context->mappings.find(key); it != context->mappings.end())
	{
		for (const input_action_t& action : it->second.actions.at(state))
		{
			action.command->execute(action.keys);
		}
	}
}