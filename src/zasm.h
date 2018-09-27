#ifndef ZASM_H
#define ZASM_H

#include <map>
#include <set>
#include <string>
#include <vector>

namespace zasm
{
	////////////////////////////////////////////////////////////////
	// Command
	
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
#		include "zasm/commands.xtable"
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

	////////////////////////////////////////////////////////////////
	// Instruction

	struct instruction
	{
		instruction() : command(cmd_terminator), arg1(0L), arg2(0L) {}
		instruction(zasm::command command, long arg1, long arg2)
			: command(command), arg1(arg1), arg2(arg2) {}
		
		zasm::command command;
		long arg1;
		long arg2;
	};
	
	////////////////////////////////////////////////////////////////
	// Scripts

	// A wrapper around a vector of instructions.
	// Ensures that the vector always ends with cmd_terminator.
	class script
	{
	public:
		typedef std::vector<instruction>::iterator iterator;
		typedef std::vector<instruction>::const_iterator const_iterator;
		
		// An standard empty script instance.
		static script const empty_instance;

		// Create an empty script.
		script();

		// Calls assign(instructions) on the newly created script.
		// The input vector is consumed.
		script(std::vector<instruction>& instructions);

		// Does this script have exactly 1 terminator, at the end?
		// (Should always be true.)
		bool is_valid() const;
		
		// Is this script empty? (Not counting terminator)
		bool empty() const;

		// Get the number of instructions, including terminator.
		std::size_t size() const;
		
		// Clear instructions.
		void clear();

		// Return the instruction at index
		instruction const& operator[](std::size_t index) const;
		
		// Get a pointer to the first instruction.
		instruction const* data() const;

		// Replace script contents with a vector of instructions.
		// Appends a terminator command if necessary.
		// The input vector is consumed.
		void assign(std::vector<instruction>& instructions);

		// Swap with other script.
		void swap(script& other);

		// Just use instructions' iterators.
		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;
		
	private:
		std::vector<instruction> instructions_;
	};

	// A name, script pair.
	class named_script
	{
	public:
		void clear() {name.clear(); script.clear();}
		bool empty() const {return name.empty() && script.empty();}

		std::string name;
		zasm::script script;
	};

	// A group of scripts of the same type.
	class script_set
	{
	public:
		////////////////////////////////////////////////////////////////
		// Iterators! - they skip over empty entries.

		class const_iterator;

		class iterator
		{
			friend class script_set;
			friend class const_iterator;
		public:
			named_script& operator*();
			named_script* operator->();
			iterator& operator++();
			bool operator==(iterator const&) const;
			bool operator!=(iterator const&) const;
			std::size_t index() const;

		private:
			iterator(std::vector<named_script>::iterator it,
			         std::vector<named_script>::iterator end,
			         std::size_t index);

			std::vector<named_script>::iterator it_;
			std::vector<named_script>::iterator end_;
			std::size_t index_;
		};

		// Skips over empty entries.
		class const_iterator
		{
			friend class script_set;
		public:
			const_iterator(iterator const&);
			
			named_script const& operator*();
			named_script const* operator->();
			const_iterator& operator++();
			bool operator==(const_iterator const&) const;
			bool operator!=(const_iterator const&) const;
			std::size_t index() const;
			
		private:
			const_iterator(std::vector<named_script>::const_iterator it,
			               std::vector<named_script>::const_iterator end,
			               std::size_t index);

			std::vector<named_script>::const_iterator it_;
			std::vector<named_script>::const_iterator end_;
			std::size_t index_;
		};
		////////////////////////////////////////////////////////////////
		
		// Create an empty set.
		script_set(std::size_t max_count);

		// Assign a name and script to a slot.
		// The input script is invalidated on success.
		// If the index is invalid, fails and returns false.
		// With no script argument, just assigns a name to the slot with no
		// code.
		bool assign(std::size_t index, std::string const& name,
		            zasm::script& script);
		bool assign(std::size_t index, std::string const& name);

		// Erase the script at position. Returns the number erased.
		std::size_t erase(std::size_t index);

		// Erase scripts by name. Returns the number of scripts erased.
		std::size_t erase(std::string const& name);

		// Get the name of the specified slot.
		std::string const& name(std::size_t index) const;

		// Get the script at the specified slot.
		zasm::script& script(std::size_t index);
		zasm::script const& script(std::size_t index) const;

		// Get the number of non-empty scripts.
		std::size_t size() const;
		
		// Remove all scripts from this set.
		void clear();

		// Iterate over contents, skipping empty ones.
		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;

		// Get an iterator for the first script with a given name.
		iterator find(std::string const& name);
		const_iterator find(std::string const& name) const;

		// The maximum number of scripts in this set.
		std::size_t const max_count;

	private:
		std::vector<named_script> data_;
		std::map<std::string, std::set<std::size_t> > name_index_;
		std::size_t size_;
	};

	// Assign multiple scripts at once by assigning each entry in names with
	// its corresponding index in scripts.
	// - Unnamed scripts are ignored.
	// - Names for invalid indices are ignored.
	// ss :: The script set to assign to.
	// scripts :: A vector of scripts. Consumed by this call.
	// names :: A map of the index in scripts to its name.
	// returns the number of assignments made.
	std::size_t assign(
		script_set& ss,
		std::vector<std::vector<instruction> >& scripts,
		std::map<std::size_t, std::string> const& names);
	
	// The set of all scripts in a quest.
	class quest_scripts
	{
	public:
		// An empty set of scripts.
		quest_scripts();

		// Clear all scripts.
		void clear();
		
		std::size_t count_global_arrays() const;
		
		script_set ffc;
		script_set item;
		script_set guy;
		script_set weapon;
		script_set screen;
		script_set global;
		script_set link;
	};
}

#endif
