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

std::map<std::string, ObjectData_Function> ObjectExpression::_function_table;

std::map<std::string, ObjectData_Register> ObjectExpression::_register_global_table;
std::map<bigsint, bool>                    ObjectExpression::_register_global_used;
std::map<std::string, ObjectData_Register> ObjectExpression::_register_map_table;
std::map<bigsint, bool>                    ObjectExpression::_register_map_used;
std::map<std::string, ObjectData_Register> ObjectExpression::_register_world_table;
std::map<bigsint, bool>                    ObjectExpression::_register_world_used;

std::map<std::string, ObjectData_RegisterArray> ObjectExpression::_registerarray_global_table;
std::map<bigsint, bool>                         ObjectExpression::_registerarray_global_used;
std::map<std::string, ObjectData_RegisterArray> ObjectExpression::_registerarray_map_table;
std::map<bigsint, bool>                         ObjectExpression::_registerarray_map_used;
std::map<std::string, ObjectData_RegisterArray> ObjectExpression::_registerarray_world_table;
std::map<bigsint, bool>                         ObjectExpression::_registerarray_world_used;

std::map<std::string, ObjectData_Script> ObjectExpression::_script_table;
std::map<bigsint, bool>                  ObjectExpression::_script_used;

ObjectExpression::Pointer                ObjectExpression:: static_offset(create_value_int(8192, SourcePosition()));
std::map<std::string, ObjectData_Static> ObjectExpression::_static_table;
std::map<bigsint, bool>                  ObjectExpression::_static_used;

std::vector<ObjectData_String> ObjectExpression::_string_table;

std::map<std::string, ObjectExpression::Pointer>        ObjectExpression::_symbol_table;
std::map<std::string, ObjectExpression::ExpressionType> ObjectExpression::_symbol_type_table;

option::option_b option_string_fold(false);



ObjectExpression::ObjectExpression(SourcePosition const & position) : position(position)
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

void ObjectExpression::add_function(std::string const & name, std::string const & label, bigsint argCount, bigsint varCount, bigsint retCount)
{
	ObjectData_Function f = {label, "", name, argCount, -1, retCount, varCount, false};
	_function_table[name] = f;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_function(std::string const & name, std::string const & label, bigsint argCount, bigsint varCount, bigsint retCount, std::string const & library)
{
	ObjectData_Function f = {label, "", name, argCount, -1, retCount, varCount, true};
	_function_table[name] = f;

	add_symbol(name, ET_INT);
}

void ObjectExpression::add_label(std::string const & symbol)
{
	add_symbol(symbol, create_value_int(_address_count, SourcePosition::none));
}

void ObjectExpression::add_register_global(std::string const & name, bigsint size)
{
	ObjectData_Register r = {name, -1, size};
	_register_global_table[name] = r;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_register_global(std::string const & name, bigsint size, bigsint number)
{
	ObjectData_Register r = {name, number, size};
	_register_global_table[name] = r;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_register_map(std::string const & name, bigsint size)
{
	ObjectData_Register r = {name, -1, size};
	_register_map_table[name] = r;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_register_map(std::string const & name, bigsint size, bigsint number)
{
	ObjectData_Register r = {name, number, size};
	_register_map_table[name] = r;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_register_world(std::string const & name, bigsint size)
{
	ObjectData_Register r = {name, -1, size};
	_register_world_table[name] = r;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_register_world(std::string const & name, bigsint size, bigsint number)
{
	ObjectData_Register r = {name, number, size};
	_register_world_table[name] = r;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_registerarray_global(std::string const & name, bigsint size)
{
	ObjectData_RegisterArray r = {name, -1, size};
	_registerarray_global_table[name] = r;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_registerarray_global(std::string const & name, bigsint size, bigsint number)
{
	ObjectData_RegisterArray r = {name, number, size};
	_registerarray_global_table[name] = r;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_registerarray_map(std::string const & name, bigsint size)
{
	ObjectData_RegisterArray r = {name, -1, size};
	_registerarray_map_table[name] = r;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_registerarray_map(std::string const & name, bigsint size, bigsint number)
{
	ObjectData_RegisterArray r = {name, number, size};
	_registerarray_map_table[name] = r;

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_registerarray_world(std::string const & name, bigsint size)
{
	ObjectData_RegisterArray r = {name, -1, size};
	_registerarray_world_table[name] = r;

	add_symbol(name, ET_INT);
}
void ObjectExpression::add_registerarray_world(std::string const & name, bigsint size, bigsint number)
{
	ObjectData_RegisterArray r = {name, number, size};
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

std::string ObjectExpression::add_string(std::string const & value)
{
	std::ostringstream oss;
	oss << "__string_" << _filename << "_" << _string_table.size();
	std::string symbol(oss.str());

	add_string(symbol, value);
	return symbol;
}
void ObjectExpression::add_string(std::string const & name, std::string const & value)
{
	ObjectData_String s = {name, value, 0};
	_string_table.push_back(s);

	add_symbol(name, ET_INT);
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

bool ObjectExpression::do_string_fold(size_t index)
{
	for (size_t i(0); i < index; ++i)
	{
		if (_string_table[index].string == _string_table[i].string)
		{
			for (size_t j(index+1); j < _string_table.size(); ++j)
				_string_table[j-1] = _string_table[j];

			_string_table.pop_back();

			return true;
		}
	}

	return false;
}

bigsint ObjectExpression::get_address_count()
{
	return _address_count;
}

std::string const & ObjectExpression::get_filename()
{
	return _filename;
}

bigsint ObjectExpression::get_register_number(std::map<bigsint, bool> * registerUsed, bigsint size)
{
	bigsint registerUsedLast(0);

	while (is_register_used(registerUsed, registerUsedLast, size) && registerUsedLast < 65535) ++registerUsedLast;

	if (registerUsedLast == 65535) throw SourceException("no more register numbers", SourcePosition::none, "ObjectExpression");

	for (bigsint i(0); i < size; ++i)
		(*registerUsed)[registerUsedLast+i] = true;

	return registerUsedLast;
}

bigsint ObjectExpression::get_registerarray_number(std::map<bigsint, bool> * registerarrayUsed)
{
	bigsint registerarrayUsedLast(0);

	while (registerarrayUsed->find(registerarrayUsedLast) != registerarrayUsed->end() && registerarrayUsedLast < 65535) ++registerarrayUsedLast;

	if (registerarrayUsedLast == 65535) throw SourceException("no more registerarray numbers", SourcePosition::none, "ObjectExpression");

	(*registerarrayUsed)[registerarrayUsedLast] = true;
	return registerarrayUsedLast;
}

bigsint ObjectExpression::get_script_count()
{
	return (bigsint)_script_table.size();
}

bigsint ObjectExpression::get_script_number()
{
	bigsint scriptUsedLast(0);

	while (_script_used.find(scriptUsedLast) != _script_used.end() && scriptUsedLast < 1000) ++scriptUsedLast;

	if (scriptUsedLast == 1000) throw SourceException("no more script numbers", SourcePosition::none, "ObjectExpression");

	_script_used[scriptUsedLast] = true;
	return scriptUsedLast;
}

bigsint ObjectExpression::get_static_number(bigsint size)
{
	return get_register_number(&_static_used, size);
}

bigsint ObjectExpression::get_string(std::string const & s)
{
	for (size_t i(0); i < _string_table.size(); ++i)
	{
		if (_string_table[i].string == s)
			return (bigsint)i;
	}

	return -1;
}

bigsint ObjectExpression::get_string_count()
{
	return (bigsint)_string_table.size();
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

void ObjectExpression::init()
{
	option::option_add("string-fold", "optimization", "Removes duplicate strings.", &option_string_fold, option::option_handler_default_b);
}

bool ObjectExpression::is_register_used(std::map<bigsint, bool> * registerUsed, bigsint number, bigsint size)
{
	if (size == 0) return false;

	std::map<bigsint, bool>::iterator it(registerUsed->find(number+size-1));

	return (it != registerUsed->end() && it->second) || is_register_used(registerUsed, number, size-1);
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

void ObjectExpression::set_address_count(bigsint addressCount)
{
	_address_count = addressCount;
}

void ObjectExpression::set_filename(std::string const & filename)
{
	_filename = filename;
}

void ObjectExpression::writeObject(std::ostream * out) const
{
	write_object(out, position);
}



void print_debug(std::ostream * out, ObjectExpression const & in)
{
	in.printDebug(out);
}



