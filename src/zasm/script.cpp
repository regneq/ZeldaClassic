#include "../zasm.h"

#include <cassert>
#include "../zdefs.h"

using namespace std;
using namespace zasm;

////////////////////////////////////////////////////////////////
// class script

script const script::empty_instance;

script::script() : instructions_(1) {}

script::script(vector<instruction>& instructions)
{
	assign(instructions);
}

bool script::is_valid() const
{
	if (instructions_.empty()) return false;
	vector<instruction>::const_reverse_iterator it = instructions_.rbegin();
	if (it->command != cmd_terminator) return false;
	for (++it; it != instructions_.rend(); ++it)
		if (it->command == cmd_terminator) return false;
	return true;
}

bool script::empty() const
{
	return instructions_.size() == 1;
}

size_t script::size() const
{
	return instructions_.size();
}

void script::clear()
{
	instructions_ = std::vector<instruction>(1);
}

instruction const& script::operator[](size_t index) const
{
	assert(index < instructions_.size());
	return instructions_[index];
}

instruction const* script::data() const
{
	return &instructions_[0];
}

void script::assign(vector<instruction>& instructions)
{
	std::swap(instructions_, instructions);
	if (instructions_.size() == 0
	    || instructions_.back().command != cmd_terminator)
		instructions_.push_back(instruction());
}

void script::swap(script& other)
{
	std::swap(instructions_, other.instructions_);
}

script::iterator script::begin()
{
	return instructions_.begin();
}

script::const_iterator script::begin() const
{
	return instructions_.begin();
}

script::iterator script::end()
{
	return instructions_.end();
}

script::const_iterator script::end() const
{
	return instructions_.end();
}

////////////////////////////////////////////////////////////////
// class script_set::iterator

named_script& script_set::iterator::operator*()
{
	return *it_;
}

named_script* script_set::iterator::operator->()
{
	return &*it_;
}

script_set::iterator& script_set::iterator::operator++()
{
	++it_;
	++index_;
	while (it_->empty() && it_ != end_)
	{
		++it_;
		++index_;
	}
	return *this;
}

bool script_set::iterator::operator==(iterator const& rhs) const
{
	return it_ == rhs.it_;
}

bool script_set::iterator::operator!=(iterator const& rhs) const
{
	return it_ != rhs.it_;
}

size_t script_set::iterator::index() const {return index_;}
			
script_set::iterator::iterator(
	vector<named_script>::iterator it,
	vector<named_script>::iterator end,
	size_t index)
	: it_(it), end_(end), index_(index)
{}

////////////////////////////////////////////////////////////////
// class script_set::const_iterator

script_set::const_iterator::const_iterator(iterator const& other)
	: it_(other.it_), end_(other.end_), index_(other.index_)
{}

named_script const& script_set::const_iterator::operator*()
{
	return *it_;
}

named_script const* script_set::const_iterator::operator->()
{
	return &*it_;
}

script_set::const_iterator& script_set::const_iterator::operator++()
{
	++it_;
	++index_;
	while (it_->empty() && it_ != end_)
	{
		++it_;
		++index_;
	}
	return *this;
}

bool script_set::const_iterator::operator==(const_iterator const& rhs) const
{
	return it_ == rhs.it_;
}

bool script_set::const_iterator::operator!=(const_iterator const& rhs) const
{
	return it_ != rhs.it_;
}

size_t script_set::const_iterator::index() const {return index_;}
			
script_set::const_iterator::const_iterator(
	vector<named_script>::const_iterator it,
	vector<named_script>::const_iterator end,
	size_t index)
	: it_(it), end_(end), index_(index)
{}

////////////////////////////////////////////////////////////////
// class script_set

script_set::script_set(size_t max_count)
	: max_count(max_count), size_(0)
{
	data_.reserve(max_count);
}

bool script_set::assign(
	size_t index, string const& name, zasm::script& script)
{
	if (index >= max_count) return false;
	if (index >= data_.size()) data_.resize(index + 1);
	
	if (name_index_[name].insert(index).second)
		++size_;

	// Steal script contents.
	data_[index].script.swap(script);

	data_[index].name = name;
	return true;
}

bool script_set::assign(size_t index, string const& name)
{
	zasm::script blank;
	return assign(index, name, blank);
}

size_t script_set::erase(size_t index)
{
	assert(index < max_count);
	if (index >= data_.size() || data_[index].empty()) return 0;

	// Remove from name index.
	string const& name = data_[index].name;
	if (!name.empty())
	{
		map<string, set<size_t> >::iterator it = name_index_.find(name);
		if (it != name_index_.end())
			it->second.erase(index);
	}

	// Remove from data.
	data_[index].clear();

	return 1;
}

size_t script_set::erase(string const& name)
{
	map<string, set<size_t> >::iterator name_it = name_index_.find(name);
	if (name_it == name_index_.end()) return 0;

	set<size_t>& indices = name_it->second;
	size_t count = indices.size();
	size_ -= count;
	
	// Clear data for the indices.
	for (set<size_t>::iterator index_it = indices.begin();
	     index_it != indices.end(); ++index_it)
		data_[*index_it].clear();

	// Remove indices from name map.
	name_index_.erase(name_it);
	
	return count;
}

namespace // file local
{
	string const empty_string_;
}

string const& script_set::name(size_t index) const
{
	assert(index < max_count);
	if (index >= data_.size()) return empty_string_;
	return data_[index].name;
}

zasm::script& script_set::script(size_t index)
{
	assert(index < max_count);
	if (index >= data_.size()) data_.resize(index + 1);
	return data_[index].script;
}

zasm::script const& script_set::script(size_t index) const
{
	assert(index < max_count);
	if (index >= data_.size()) return script::empty_instance;
	return data_[index].script;
}

size_t script_set::size() const
{
	return size_;
}

void script_set::clear()
{
	data_.clear();
	name_index_.clear();
	size_ = 0;
}

script_set::iterator script_set::begin()
{
	return iterator(data_.begin(), data_.end(), 0);
}

script_set::const_iterator script_set::begin() const
{
	return const_iterator(data_.begin(), data_.end(), 0);
}

script_set::iterator script_set::end()
{
	return iterator(data_.end(), data_.end(), data_.size());
}

script_set::const_iterator script_set::end() const
{
	return const_iterator(data_.end(), data_.end(), data_.size());
}

script_set::iterator script_set::find(std::string const& name)
{
	map<string, set<size_t> >::const_iterator it = name_index_.find(name);
	if (it == name_index_.end() || it->second.empty()) return end();
	size_t index = *it->second.begin();
	return iterator(data_.begin() + index, data_.end(), index);
}

script_set::const_iterator script_set::find(std::string const& name) const
{
	map<string, set<size_t> >::const_iterator it = name_index_.find(name);
	if (it == name_index_.end() || it->second.empty()) return end();
	size_t index = *it->second.begin();
	return const_iterator(data_.begin() + index, data_.end(), index);
}


size_t zasm::assign(script_set& ss, vector<vector<instruction> >& scripts,
                    map<size_t, string> const& names)
{
	size_t count = 0;

	for (map<size_t, string>::const_iterator it = names.begin();
	     it != names.end(); ++it)
	{
		// Skip invalid indices.
		if (it->first >= ss.max_count) continue;
		
		// Skip empty names.
		if (it->second.empty()) continue;

		zasm::script script(scripts[it->first]);
		ss.assign(it->first, it->second, script);
		++count;
	}

	return count;
}

////////////////////////////////////////////////////////////////
// class quest_scripts

quest_scripts::quest_scripts()
	: ffc(NUMSCRIPTFFC),
	  item(NUMSCRIPTITEM),
	  guy(NUMSCRIPTGUY),
	  weapon(NUMSCRIPTWEAPON),
	  screen(NUMSCRIPTSCREEN),
	  global(NUMSCRIPTGLOBAL),
	  link(NUMSCRIPTLINK)
{}

void quest_scripts::clear()
{
	ffc.clear();
	item.clear();
	guy.clear();
	weapon.clear();
	screen.clear();
	global.clear();
	link.clear();
}

size_t quest_scripts::count_global_arrays() const
{
	script const& init = global.script(GLOBAL_SCRIPT_INIT);
	size_t count = 0;

	for (script::const_iterator it = init.begin();
	     it != init.end() && it->command != cmd_terminator;
	     ++it)
		if (it->command == cmd_ALLOCATEGMEMV
		    || it->command == cmd_ALLOCATEGMEMR)
			++count;
    
    return count;
}

