#ifndef ZASM_COMMAND_H
#define ZASM_COMMAND_H

#include <string>

namespace zasm
{
	enum arg_type
	{
		arg_none,
		arg_register,
		arg_immediate,
		arg_label
	};

	enum command
	{
#		define X(NAME, ARG1, ARG2) cmd_##NAME,
#		include "commands.xtable"
#		undef X
		command_count,
		cmd_terminator = 0xFFFF
	};

	command get_command(std::string const& name);
	bool is_valid(command);
	std::string const& get_name(command);
	arg_type get_first_argtype(command);
	arg_type get_second_argtype(command);
	int get_arg_count(command);
}

#endif
