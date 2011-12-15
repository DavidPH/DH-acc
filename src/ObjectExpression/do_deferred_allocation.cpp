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

#include "iter.hpp"

#include "../ost_type.hpp"
#include "../SourceException.hpp"



typedef std::set<bigsint> UsedMap;



//
// is_used
//
// Returns true is a section of memory is unavailable for allocation.
//
static bool is_used(UsedMap * used, bigsint number, bigsint size)
{
	if (size == 0) return false;

	return used->count(number+size-1) || is_used(used, number, size-1);
}

//
// set_used
//
// Marks a section of memory as unavailable for allocation.
//
static void set_used(UsedMap * used, bigsint number, bigsint size)
{
	for (bigsint i(0); i < size; ++i)
		used->insert(number+i);
}

//
// get_used
//
// Allocates a section of memory.
//
static bigsint get_used(UsedMap * used, bigsint size)
{
	bigsint number(0);

	while (is_used(used, number, size) && number < 65535) ++number;

	if (number == 65535)
		throw SourceException("no more used", SourcePosition::none(), "ObjectExpression");

	set_used(used, number, size);

	return number;
}

//
// allocate_pre
//
template<typename T> static void allocate_pre(UsedMap * used, T & data)
{
	if (data.number != -1)
		set_used(used, data.number, 1);
}

//
// allocate_pre_size
//
template<typename T> static void allocate_pre_size(UsedMap * used, T & data)
{
	if (data.number != -1)
		set_used(used, data.number, data.size);
}

//
// allocate
//
template<typename T> static void allocate(UsedMap * used, T & data)
{
	if (data.number == -1)
		data.number = get_used(used, 1);

	ObjectExpression::add_symbol(data.name, ObjectExpression::create_value_int(data.number, SourcePosition::none()));
}

//
// allocate_size
//
template<typename T> static void allocate_size(UsedMap * used, T & data)
{
	if (data.number == -1)
		data.number = get_used(used, data.size);

	ObjectExpression::add_symbol(data.name, ObjectExpression::create_value_int(data.number, SourcePosition::none()));
}

//
// allocate_Function
//
static void allocate_Function(bigsint * number, ObjectData_Function & f)
{
	f.number = (*number)++;

	ObjectExpression::add_symbol(f.name, ObjectExpression::create_value_int(f.number, SourcePosition::none()));
}

void ObjectExpression::do_deferred_allocation()
{
	UsedMap used;


	// mapregisters and maparrays
	if (output_type == OUTPUT_ACSE)
	{
		// ACSE's MEXP uses the same addresses for both.
		// TODO: !externVis registers/arrays should not reserve the
		// corresponding arrays/registers.
		used.clear();

		_iterator_map(_register_map_table, allocate_pre_size<ObjectData_Register>, &used);
		_iterator_map(_registerarray_map_table, allocate_pre<ObjectData_RegisterArray>, &used);
		_iterator_map(_register_map_table, allocate_size<ObjectData_Register>, &used);
		_iterator_map(_registerarray_map_table, allocate<ObjectData_RegisterArray>, &used);
	}
	else
	{
		// mapregisters
		used.clear();

		_iterator_map(_register_map_table, allocate_pre_size<ObjectData_Register>, &used);
		_iterator_map(_register_map_table, allocate_size<ObjectData_Register>, &used);

		// maparrays
		used.clear();

		_iterator_map(_registerarray_map_table, allocate_pre<ObjectData_RegisterArray>, &used);
		_iterator_map(_registerarray_map_table, allocate<ObjectData_RegisterArray>, &used);
	}


	// worldregisters
	used.clear();
	set_used(&used, 0, 1); // Stack pointer.
	set_used(&used, 1, 1); // Array temporary.

	_iterator_map(_register_world_table, allocate_pre_size<ObjectData_Register>, &used);
	_iterator_map(_register_world_table, allocate_size<ObjectData_Register>, &used);


	// globalregisters
	used.clear();

	_iterator_map(_register_global_table, allocate_pre_size<ObjectData_Register>, &used);
	_iterator_map(_register_global_table, allocate_size<ObjectData_Register>, &used);


	// worldarrays
	used.clear();

	_iterator_map(_registerarray_world_table, allocate_pre<ObjectData_RegisterArray>, &used);
	_iterator_map(_registerarray_world_table, allocate<ObjectData_RegisterArray>, &used);


	// globalarrays
	used.clear();
	set_used(&used, 0, 1); // Addressable variables.

	_iterator_map(_registerarray_global_table, allocate_pre<ObjectData_RegisterArray>, &used);
	_iterator_map(_registerarray_global_table, allocate<ObjectData_RegisterArray>, &used);


	// For ACS+, all the following allocation is done by the linker.
	if (output_type == OUTPUT_ACSP) return;


	// functions
	if (target_type == TARGET_ZDoom)
	{
		bigsint number(0);

		_iterator_map(_function_table, allocate_Function, &number);
	}


	// scripts
	used.clear();

	_iterator_map(_script_table, allocate_pre<ObjectData_Script>, &used);
	_iterator_map(_script_table, allocate<ObjectData_Script>, &used);


	// statics
	used.clear();

	_iterator_map(_static_table, allocate_pre<ObjectData_Static>, &used);
	_iterator_map(_static_table, allocate<ObjectData_Static>, &used);


	// strings
	ObjectData_String::generate_symbols();
}


