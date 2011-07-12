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

#include <sstream>



int32_t ObjectExpression::_address_count(8+4+4+12);
std::vector<std::pair<std::string, int32_t> > ObjectExpression::_string_table;
std::map<std::string, ObjectExpression> ObjectExpression::_symbol_table;



ObjectExpression::ObjectExpression() : _expr(NULL)
{

}
ObjectExpression::ObjectExpression(ObjectExpression const & expr) : _expr(expr._expr ? expr._expr->clone() : NULL)
{

}
ObjectExpression::ObjectExpression(ObjectExpressionBase * const expr) : _expr(expr)
{

}
ObjectExpression::~ObjectExpression()
{
	delete _expr;
}

void ObjectExpression::add_address_count(int32_t const addressCount)
{
	_address_count += addressCount;
}

void ObjectExpression::add_label(std::string const & symbol)
{
	add_symbol(symbol, create_value_int32(_address_count, SourcePosition::none));
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

	add_address_count(4 + (int32_t)value.size());
	add_symbol(symbol, create_value_int32((int32_t)_string_table.size(), SourcePosition::none));

	_string_table.push_back(std::pair<std::string, int32_t>(value, get_string_length()));
}

void ObjectExpression::add_symbol(std::string const & symbol, ObjectExpression const & value)
{
	_symbol_table[symbol] = value;
}

std::string const & ObjectExpression::get_string(int32_t const index)
{
	return _string_table[index].first;
}

int32_t ObjectExpression::get_string_count()
{
	return (int32_t)_string_table.size();
}

int32_t ObjectExpression::get_string_length()
{
	if (_string_table.empty()) return 0;

	return _string_table.back().second + _string_table.back().first.size();
}

int32_t ObjectExpression::get_string_offset(int32_t const index)
{
	return _string_table[index].second;
}

ObjectExpression ObjectExpression::get_symbol(std::string const & symbol, SourcePosition const & position)
{
	std::map<std::string, ObjectExpression>::iterator valueIt(_symbol_table.find(symbol));

	if (valueIt == _symbol_table.end())
		throw SourceException("unknown symbol", position, "ObjectExpression");

	return valueIt->second;
}

SourcePosition const & ObjectExpression::getPosition() const
{
	return _expr->getPosition();
}

ObjectExpression & ObjectExpression::operator = (ObjectExpression const & expr)
{
	delete _expr;
	_expr = expr._expr ? expr._expr->clone() : NULL;
	return *this;
}

int32_t ObjectExpression::resolveInt32() const
{
	return _expr ? _expr->resolveInt32() : 0;
}



void print_debug(std::ostream * const out, ObjectExpression const & in)
{
	*out << "ObjectExpression(";

	if (in._expr)
		//in._expr->printDebug(out);
		*out << "TODO";
	else
		*out << "NULL";

	*out << ")";
}



