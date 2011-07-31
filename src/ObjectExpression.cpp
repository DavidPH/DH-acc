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
** ObjectExpression methods.
*/

#include "ObjectExpression.hpp"

#include "SourceException.hpp"
#include "SourceTokenC.hpp"

#include <sstream>



std::vector<ObjectExpression::ACSFunc> ObjectExpression::_acsfunc_table;
int32_t ObjectExpression::_address_count;
std::vector<ObjectExpression::RegisterArray> ObjectExpression::_registerarray_map_table;
std::vector<ObjectExpression::Script> ObjectExpression::_script_table;
std::map<ObjectExpression::int_t, bool> ObjectExpression::_script_used;
std::vector<ObjectExpression::String> ObjectExpression::_string_table;
std::map<std::string, ObjectExpression::Pointer> ObjectExpression::_symbol_table;



ObjectExpression::ObjectExpression(SourcePosition const & position) : position(position)
{

}

void ObjectExpression::add_acsfunc(std::string const & label, int_t argCount, int_t varCount, int_t retCount)
{
	ACSFunc f = {argCount, label, retCount, varCount};
	_acsfunc_table.push_back(f);
}

void ObjectExpression::add_address_count(int32_t const addressCount)
{
	_address_count += addressCount;
}

void ObjectExpression::add_label(std::string const & symbol)
{
	add_symbol(symbol, create_value_int(_address_count, SourcePosition::none));
}

void ObjectExpression::add_registerarray_map(std::string const & name, int_t number, int_t size)
{
	RegisterArray r = {name, number, size};
	_registerarray_map_table.push_back(r);

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

void ObjectExpression::add_script(std::string const & name, std::string const & label, ScriptType stype, int_t flags, int_t argCount, int_t varCount)
{
	Script s = {label, name, stype, argCount, flags, -1, varCount};
	_script_table.push_back(s);
}
void ObjectExpression::add_script(std::string const & name, std::string const & label, ScriptType stype, int_t flags, int_t argCount, int_t varCount, int_t number)
{
	reserve_script_number(number);
	Script s = {label, name, stype, argCount, flags, number, varCount};
	_script_table.push_back(s);

	add_symbol(name, create_value_int(number, SourcePosition::none));
}

std::string ObjectExpression::add_string(std::string const & value)
{
	std::ostringstream oss;
	oss << "__string_" << _string_table.size();
	std::string symbol(oss.str());

	add_string(symbol, value);
	return symbol;
}
void ObjectExpression::add_string(std::string const & symbol, std::string const & value)
{
	// TODO: Option for string folding.

	add_symbol(symbol, create_value_int((int_t)_string_table.size(), SourcePosition::none));

	String s = {get_string_length(), value};
	_string_table.push_back(s);
}

void ObjectExpression::add_symbol(std::string const & symbol, ObjectExpression * value)
{
	_symbol_table[symbol] = value;
}

void ObjectExpression::do_deferred_allocation()
{
	for (size_t i(0); i < _script_table.size(); ++i)
	{
		if (_script_table[i].number == -1)
		{
			_script_table[i].number = get_script_number();

			add_symbol(_script_table[i].name, create_value_int(_script_table[i].number, SourcePosition::none));
		}
	}
}

ObjectExpression::ACSFunc const & ObjectExpression::get_acsfunc(int_t const index)
{
	return _acsfunc_table[index];
}

ObjectExpression::int_t ObjectExpression::get_acsfunc_count()
{
	return _acsfunc_table.size();
}

ObjectExpression::float_t ObjectExpression::get_float(SourceTokenC const & token)
{
	float_t f;
	std::istringstream iss(token.getData());
	iss >> f;
	return f;
}

ObjectExpression::int_t ObjectExpression::get_int(SourceTokenC const & token)
{
	int_t i;
	std::istringstream iss(token.getData());
	iss >> i;
	return i;
}

ObjectExpression::RegisterArray const & ObjectExpression::get_registerarray_map(int_t index)
{
	return _registerarray_map_table[index];
}

ObjectExpression::int_t ObjectExpression::get_registerarray_map_count()
{
	return (int_t)_registerarray_map_table.size();
}

ObjectExpression::ScriptFlag ObjectExpression::get_ScriptFlag(std::string const & value, SourcePosition const & position)
{
	if (value == "net")
		return SF_NET;

	if (value == "clientside")
		return SF_CLIENTSIDE;

	throw SourceException("invalid script-flag", position, "ObjectExpression");
}

ObjectExpression::ScriptType ObjectExpression::get_ScriptType(std::string const & value, SourcePosition const & position)
{
	if (value == "closed")
		return ST_CLOSED;

	if (value == "open")
		return ST_OPEN;

	if (value == "respawn")
		return ST_RESPAWN;

	if (value == "death")
		return ST_DEATH;

	if (value == "enter")
		return ST_ENTER;

	if (value == "lightning")
		return ST_LIGHTNING;

	if (value == "unloading")
		return ST_UNLOADING;

	if (value == "disconnect")
		return ST_DISCONNECT;

	if (value == "return")
		return ST_RETURN;

	throw SourceException("invalid script-type", position, "ObjectExpression");
}

ObjectExpression::Script const & ObjectExpression::get_script(int32_t const index)
{
	return _script_table[index];
}

int32_t ObjectExpression::get_script_count()
{
	return (int32_t)_script_table.size();
}

ObjectExpression::int_t ObjectExpression::get_script_number()
{
	int_t scriptUsedLast(0);

	while (_script_used.find(scriptUsedLast) != _script_used.end() && scriptUsedLast < 1000) ++scriptUsedLast;

	if (scriptUsedLast == 1000) throw SourceException("no more script numbers", SourcePosition::none, "ObjectExpression");

	_script_used[scriptUsedLast] = true;
	return scriptUsedLast;
}

ObjectExpression::String const & ObjectExpression::get_string(int32_t const index)
{
	return _string_table[index];
}

int32_t ObjectExpression::get_string_count()
{
	return (int32_t)_string_table.size();
}

int32_t ObjectExpression::get_string_length()
{
	if (_string_table.empty()) return 0;

	return _string_table.back().offset + _string_table.back().string.size();
}

ObjectExpression::Pointer ObjectExpression::get_symbol(std::string const & symbol, SourcePosition const & position)
{
	std::map<std::string, ObjectExpression::Pointer>::iterator valueIt(_symbol_table.find(symbol));

	if (valueIt == _symbol_table.end())
		throw SourceException("unknown symbol '" + symbol + "'", position, "ObjectExpression");

	return valueIt->second;
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

void ObjectExpression::reserve_script_number(int32_t number)
{
	_script_used[number] = true;
}

ObjectExpression::float_t ObjectExpression::resolveFloat() const
{
	throw SourceException("cannot resolve float", position, getName());
}
ObjectExpression::int_t ObjectExpression::resolveInt() const
{
	throw SourceException("cannot resolve int", position, getName());
}

void ObjectExpression::set_address_count(int32_t addressCount)
{
	_address_count = addressCount;
}



void print_debug(std::ostream * out, ObjectExpression const & in)
{
	in.printDebug(out);
}



