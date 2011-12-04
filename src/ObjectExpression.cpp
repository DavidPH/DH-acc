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

/* ObjectExpression.cpp
**
** Defines the ObjectExpression methods.
*/

#include "ObjectExpression.hpp"

#include "ObjectCode.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"

#include <sstream>



bigsint ObjectExpression::_address_count;

std::string ObjectExpression::_filename;

std::string ObjectExpression::_library_original;
std::set<std::string> ObjectExpression::_library_table;

std::map<std::string, ObjectData_Auto> ObjectExpression::_auto_table;

std::map<std::string, ObjectData_Function> ObjectExpression::_function_table;

std::map<std::string, ObjectData_Register> ObjectExpression::_register_global_table;
std::map<std::string, ObjectData_Register> ObjectExpression::_register_map_table;
std::map<std::string, ObjectData_Register> ObjectExpression::_register_world_table;

std::map<std::string, ObjectData_RegisterArray> ObjectExpression::_registerarray_global_table;
std::map<std::string, ObjectData_RegisterArray> ObjectExpression::_registerarray_map_table;
std::map<std::string, ObjectData_RegisterArray> ObjectExpression::_registerarray_world_table;

std::map<std::string, ObjectData_Script> ObjectExpression::_script_table;

ObjectExpression::Pointer                ObjectExpression:: static_offset(create_value_int(8192, SourcePosition()));
std::map<std::string, ObjectData_Static> ObjectExpression::_static_table;

std::map<std::string, ObjectExpression::Pointer>        ObjectExpression::_symbol_table;
std::map<std::string, ObjectExpression::ExpressionType> ObjectExpression::_symbol_type_table;

option_data<bool> option_string_fold("string-fold", "optimization", "Removes duplicate strings.", false);



ObjectExpression::ObjectExpression(SourcePosition const & position_) : position(position_)
{

}
ObjectExpression::ObjectExpression(std::istream * in)
{
	read_object(in, &position);
}

void ObjectExpression::add_address_count(bigsint const addressCount)
{
	_address_count += addressCount;
}

void ObjectExpression::add_auto(std::string const & name, bigsint size, bigsint number)
{
	ObjectData_Auto s = {name, number, size};
	_auto_table[name] = s;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_function(std::string const & name, std::string const & label, bigsint argCount, bigsint varCount, bigsint retCount, bool externDef)
{
	ObjectData_Function f = {label, name, argCount, -1, retCount, varCount, externDef};
	_function_table[name] = f;

	add_symbol(name, ET_INT);
}

void ObjectExpression::add_label(std::string const & symbol)
{
	add_symbol(symbol, create_value_int(_address_count, SourcePosition::none));
}

void ObjectExpression::add_register_global(std::string const & name, bigsint size, bool externDef, bool externVis)
{
	ObjectData_Register r = {name, -1, size, externDef, externVis};
	_register_global_table[name] = r;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_register_global(std::string const & name, bigsint size, bool externDef, bool externVis, bigsint number)
{
	ObjectData_Register r = {name, number, size, externDef, externVis};
	_register_global_table[name] = r;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_register_map(std::string const & name, bigsint size, bool externDef, bool externVis)
{
	ObjectData_Register r = {name, -1, size, externDef, externVis};
	_register_map_table[name] = r;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_register_map(std::string const & name, bigsint size, bool externDef, bool externVis, bigsint number)
{
	ObjectData_Register r = {name, number, size, externDef, externVis};
	_register_map_table[name] = r;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_register_world(std::string const & name, bigsint size, bool externDef, bool externVis)
{
	ObjectData_Register r = {name, -1, size, externDef, externVis};
	_register_world_table[name] = r;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_register_world(std::string const & name, bigsint size, bool externDef, bool externVis, bigsint number)
{
	ObjectData_Register r = {name, number, size, externDef, externVis};
	_register_world_table[name] = r;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_registerarray_global(std::string const & name, bigsint size, bool externDef, bool externVis)
{
	ObjectData_RegisterArray r = {name, -1, size, externDef, externVis};
	_registerarray_global_table[name] = r;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_registerarray_global(std::string const & name, bigsint size, bool externDef, bool externVis, bigsint number)
{
	ObjectData_RegisterArray r = {name, number, size, externDef, externVis};
	_registerarray_global_table[name] = r;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_registerarray_map(std::string const & name, bigsint size, bool externDef, bool externVis)
{
	ObjectData_RegisterArray r = {name, -1, size, externDef, externVis};
	_registerarray_map_table[name] = r;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_registerarray_map(std::string const & name, bigsint size, bool externDef, bool externVis, bigsint number)
{
	ObjectData_RegisterArray r = {name, number, size, externDef, externVis};
	_registerarray_map_table[name] = r;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_registerarray_world(std::string const & name, bigsint size, bool externDef, bool externVis)
{
	ObjectData_RegisterArray r = {name, -1, size, externDef, externVis};
	_registerarray_world_table[name] = r;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_registerarray_world(std::string const & name, bigsint size, bool externDef, bool externVis, bigsint number)
{
	ObjectData_RegisterArray r = {name, number, size, externDef, externVis};
	_registerarray_world_table[name] = r;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_script(std::string const & name, std::string const & label, ObjectData_Script::ScriptType stype, bigsint flags, bigsint argCount, bigsint varCount)
{
	ObjectData_Script s = {label, name, stype, argCount, flags, -1, varCount};
	_script_table[name] = s;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_script(std::string const & name, std::string const & label, ObjectData_Script::ScriptType stype, bigsint flags, bigsint argCount, bigsint varCount, bigsint number)
{
	ObjectData_Script s = {label, name, stype, argCount, flags, number, varCount};
	_script_table[name] = s;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_static(std::string const & name, bigsint size)
{
	ObjectData_Static s = {name, -1, size};
	_static_table[name] = s;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_static(std::string const & name, bigsint size, bigsint number)
{
	ObjectData_Static s = {name, number, size};
	_static_table[name] = s;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_symbol(std::string const & symbol, ObjectExpression * value)
{
	add_symbol(symbol, value->getType());
	_symbol_table[symbol] = value;
}
void ObjectExpression::add_symbol(std::string const & symbol, ExpressionType type)
{
	_symbol_type_table[symbol] = type;
}

bigsint ObjectExpression::get_address_count()
{
	return _address_count;
}

std::string const & ObjectExpression::get_filename()
{
	return _filename;
}

ObjectExpression::Pointer ObjectExpression::get_symbol(std::string const & symbol, SourcePosition const & position)
{
	std::map<std::string, ObjectExpression::Pointer>::iterator valueIt(_symbol_table.find(symbol));

	if (valueIt == _symbol_table.end())
		throw SourceException("unknown symbol '" + symbol + "'", position, "ObjectExpression");

	return valueIt->second;
}
ObjectExpression::ExpressionType ObjectExpression::get_symbol_type(std::string const & symbol, SourcePosition const & position)
{
	std::map<std::string, ExpressionType>::iterator typeIt(_symbol_type_table.find(symbol));

	if (typeIt == _symbol_type_table.end())
		throw SourceException("unknown symbol '" + symbol + "'", position, "ObjectExpression");

	return typeIt->second;
}

SourcePosition const & ObjectExpression::getPosition() const
{
	return position;
}

void ObjectExpression::printDebug(std::ostream * const out) const
{
	*out << "ObjectExpression(";
		*out << "position=(";
		print_debug(out, position);
		*out << ")";
	*out << ")";
}

bigreal ObjectExpression::resolveFloat() const
{
	throw SourceException("cannot resolve float", position, getName());
}
bigsint ObjectExpression::resolveInt() const
{
	throw SourceException("cannot resolve int", position, getName());
}
ObjectCodeSet ObjectExpression::resolveOCode() const
{
	throw SourceException("cannot resolve ocode", position, getName());
}

void ObjectExpression::writeACSP(std::ostream * out) const
{
	throw SourceException("cannot write ACS+", position, getName());
}

void ObjectExpression::set_address_count(bigsint addressCount)
{
	_address_count = addressCount;
}

void ObjectExpression::set_filename(std::string const & filename)
{
	_filename = filename;
}

void ObjectExpression::set_library(std::string const & library)
{
	if (_library_original.empty())
		_library_original = library;

	_library_table.insert(library);
}

void ObjectExpression::writeObject(std::ostream * out) const
{
	write_object(out, position);
}



void print_debug(std::ostream * out, ObjectExpression const & in)
{
	in.printDebug(out);
}


