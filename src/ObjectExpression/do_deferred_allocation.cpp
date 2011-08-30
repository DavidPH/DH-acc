/* Copyright (C) 2011 David Hill
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* ObjectExpression/do_deferred_allocation.cpp
**
** Defines the ObjectExpression::do_deferred_allocation function.
*/

#include "../ObjectExpression.hpp"



void ObjectExpression::do_deferred_allocation()
{
	// Deferred function allocation.
	// TODO: libraries
	// TODO: explicit allocation
	{
		bigsint number(0);

		for (std::map<std::string, ObjectData_Function>::iterator it(_function_table.begin()); it != _function_table.end(); ++it)
		{
			ObjectData_Function & f(it->second);

			f.number = number++;

			add_symbol(f.name, create_value_int(f.number, SourcePosition::none));
		}
	}

	// Stack pointer.
	_register_world_used[0] = true;
	// Array temporary.
	_register_world_used[1] = true;

	// Pointer-addressable space.
	_registerarray_global_used[0] = true;

	// Deferred register allocation.
	do_deferred_allocation_register(&_register_global_table, &_register_global_used);
	do_deferred_allocation_register(&_register_map_table, &_register_map_used);
	do_deferred_allocation_register(&_register_world_table, &_register_world_used);

	// Deferred registerarray allocation.
	do_deferred_allocation_registerarray(&_registerarray_global_table, &_registerarray_global_used);
	do_deferred_allocation_registerarray(&_registerarray_map_table, &_registerarray_map_used);
	do_deferred_allocation_registerarray(&_registerarray_world_table, &_registerarray_world_used);

	// Deferred script allocation.
	for (std::map<std::string, ObjectData_Script>::iterator it(_script_table.begin()); it != _script_table.end(); ++it)
	{
		ObjectData_Script & s(it->second);

		if (s.number != -1)
			_script_used[s.number] = true;
	}

	for (std::map<std::string, ObjectData_Script>::iterator it(_script_table.begin()); it != _script_table.end(); ++it)
	{
		ObjectData_Script & s(it->second);

		if (s.number == -1)
		{
			s.number = get_script_number();

			add_symbol(s.name, create_value_int(s.number, SourcePosition::none));
		}
	}

	// Deferred static allocation.
	for (std::map<std::string, ObjectData_Static>::iterator it(_static_table.begin()); it != _static_table.end(); ++it)
	{
		ObjectData_Static & s(it->second);

		if (s.number != -1)
		{
			for (bigsint j(0); j < s.size; ++j)
				_static_used[s.number+j] = true;
		}
	}

	for (std::map<std::string, ObjectData_Static>::iterator it(_static_table.begin()); it != _static_table.end(); ++it)
	{
		ObjectData_Static & s(it->second);

		if (s.number == -1)
		{
			s.number = get_static_number(s.size);

			add_symbol(s.name, create_value_int(s.number, SourcePosition::none));
		}
	}

	// Deferred string allocation.
	for (size_t i(0); i < _string_table.size(); ++i)
	{
		if (option_string_fold.data)
			add_symbol(_string_table[i].name, create_value_int(get_string(_string_table[i].string), SourcePosition::none));
		else
			add_symbol(_string_table[i].name, create_value_int((bigsint)i, SourcePosition::none));
	}
	if (option_string_fold.data) for (size_t i(0); i < _string_table.size();)
	{
		if (do_string_fold(i))
			continue;

		++i;
	}
	// Wait until after possible folding to do offset calculations.
	if (!_string_table.empty())
		_string_table[0].offset = 0;
	for (size_t i(1); i < _string_table.size(); ++i)
		_string_table[i].offset = _string_table[i-1].offset + _string_table[i-1].string.size();
}
void ObjectExpression::do_deferred_allocation_register(std::map<std::string, ObjectData_Register> * registerTable, std::map<bigsint, bool> * registerUsed)
{
	for (std::map<std::string, ObjectData_Register>::iterator it(registerTable->begin()); it != registerTable->end(); ++it)
	{
		ObjectData_Register & r(it->second);

		if (r.number != -1)
		{
			for (bigsint j(0); j < r.size; ++j)
				(*registerUsed)[r.number+j] = true;
		}
	}

	for (std::map<std::string, ObjectData_Register>::iterator it(registerTable->begin()); it != registerTable->end(); ++it)
	{
		ObjectData_Register & r(it->second);

		if (r.number == -1)
		{
			r.number = get_register_number(registerUsed, r.size);

			add_symbol(r.name, create_value_int(r.number, SourcePosition::none));
		}
	}
}
void ObjectExpression::do_deferred_allocation_registerarray(std::map<std::string, ObjectData_RegisterArray> * registerarrayTable, std::map<bigsint, bool> * registerarrayUsed)
{
	for (std::map<std::string, ObjectData_RegisterArray>::iterator it(registerarrayTable->begin()); it != registerarrayTable->end(); ++it)
	{
		ObjectData_RegisterArray & r(it->second);

		if (r.number != -1)
			(*registerarrayUsed)[r.number] = true;
	}

	for (std::map<std::string, ObjectData_RegisterArray>::iterator it(registerarrayTable->begin()); it != registerarrayTable->end(); ++it)
	{
		ObjectData_RegisterArray & r(it->second);

		if (r.number == -1)
		{
			r.number = get_registerarray_number(registerarrayUsed);

			add_symbol(r.name, create_value_int(r.number, SourcePosition::none));
		}
	}
}


