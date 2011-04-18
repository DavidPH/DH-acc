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

/* ObjectToken.cpp
**
** ObjectToken methods.
*/

#include "ObjectToken.hpp"

#include "SourceException.hpp"



std::vector<std::pair<std::string, int32_t> > ObjectToken::_string_table;
std::map<std::string, int32_t> ObjectToken::_symbol_table;



ObjectToken::ObjectToken(ObjectCode const code, SourcePosition const & position, std::vector<int32_t> const & args) : _args(args), _code(code), _position(position)
{

}

void ObjectToken::add_string(std::string const & symbol, std::string const & value)
{
	// TODO: Option for string folding.

	add_symbol(symbol, (int32_t)_string_table.size());

	_string_table.push_back(std::pair<std::string, int32_t>(value, get_string_length()));
}

void ObjectToken::add_symbol(std::string const & symbol, int32_t const value)
{
	_symbol_table[symbol] = value;
}

int32_t ObjectToken::getArgInt32(uintptr_t const index) const
{
	if (index < _args.size())
		return _args[index];
	else
		return 0;
}

ObjectToken::ObjectCode ObjectToken::getCode() const
{
	return _code;
}

SourcePosition const & ObjectToken::getPosition() const
{
	return _position;
}

std::string const & ObjectToken::get_string(int32_t const index)
{
	return _string_table[index].first;
}

int32_t ObjectToken::get_string_count()
{
	return (int32_t)_string_table.size();
}

int32_t ObjectToken::get_string_length()
{
	if (_string_table.empty()) return 0;

	return _string_table.back().second + _string_table.back().first.size();
}

int32_t ObjectToken::get_string_offset(int32_t const index)
{
	return _string_table[index].second;
}

int32_t ObjectToken::get_symbol(std::string const & symbol, SourcePosition const & position)
{
	std::map<std::string, int32_t>::iterator valueIt(_symbol_table.find(symbol));

	if (valueIt == _symbol_table.end())
		throw SourceException("unknown symbol", position, "ObjectToken");

	return valueIt->second;
}



