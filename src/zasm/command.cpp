#include "command.h"

#include <cctype>
#include <map>
#include "../string_utils.h"

using namespace zc;
using namespace std;
using namespace zasm;

namespace // file local
{
	////////////////////////////////////////////////////////////////
	// Command definitions

	class command_def
	{
	public:
		string name;
		arg_type first_arg;
		arg_type second_arg;
	};

	command_def definitions[command_count + 1] =
	{
#		define ARG_NONE arg_none
#		define ARG_REG arg_register
#		define ARG_IMM arg_immediate
#		define ARG_LBL arg_label
#		define X(NAME, ARG1, ARG2) \
		{#NAME, ARG_##ARG1, ARG_##ARG2},
#		include "commands.xtable"
#		undef ARG_NONE
#		undef ARG_REG
#		undef ARG_IMM
#		undef ARG_LBL
#		undef X
		{"", arg_none, arg_none}
	};

	////////////////////////////////////////////////////////////////
	
	string const invalid_name = "<invalid command>";
}

////////////////////////////////////////////////////////////////
// command

command zasm::get_command(string const& name)
{
	typedef map<string, command> map_type;
	typedef map_type::const_iterator iter_type;

	// Initialize the map the first time this is run.
	static map_type map;
	if (map.empty())
	{
#		define X(NAME, ARG1, ARG2) map[#NAME] = cmd_##NAME;
#		include "commands.xtable"
#		undef X	
	}

	iter_type it = map.find(to_upper(name));
	if (it == map.end()) return cmd_terminator;
	else return command(it->second);
}

bool zasm::is_valid(command com)
{
	return com >= 0 && com < command_count;
}

string const& get_name(command com)
{
	if (com < 0 || com >= command_count) return invalid_name;
	return definitions[com].name;
}

arg_type zasm::get_first_argtype(command com)
{
	if (com < 0 || com >= command_count) return arg_none;
	return definitions[com].first_arg;
}

arg_type zasm::get_second_argtype(command com)
{
	if (com < 0 || com >= command_count) return arg_none;
	return definitions[com].second_arg;
}

int zasm::get_arg_count(command com)
{
	if (get_first_argtype(com) == arg_none) return 0;
	if (get_second_argtype(com) == arg_none) return 1;
	return 2;
}
