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
** Defines the ObjectToken methods.
*/

#include "ObjectToken.hpp"

#include "ObjectExpression.hpp"
#include "object_io.hpp"
#include "print_debug.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"

#include <algorithm>



ObjectToken::ObjectToken() : _code(OCODE_NONE)
{

}
ObjectToken::ObjectToken(ObjectCode const code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<ObjectExpression::Pointer> const & args) : _args(args), _code(code), _labels(labels), _position(position)
{

}

void ObjectToken::addLabel(std::string const & label)
{
	_labels.push_back(label);
}
void ObjectToken::addLabel(std::vector<std::string> const & labels)
{
	for (size_t i(0); i < labels.size(); ++i)
		addLabel(labels[i]);
}

std::vector<ObjectExpression::Pointer> const & ObjectToken::getArgs() const
{
	return _args;
}
ObjectExpression::Pointer ObjectToken::getArg(size_t index) const
{
	static ObjectExpression::Pointer expr(ObjectExpression::create_value_int(0, SourcePosition::none));

	if (index < _args.size())
		return _args[index];
	else
		return expr;
}

ObjectCode ObjectToken::getCode() const
{
	return _code;
}

std::vector<std::string> const & ObjectToken::getLabels() const
{
	return _labels;
}

SourcePosition const & ObjectToken::getPosition() const
{
	return _position;
}

void ObjectToken::swapData(ObjectToken & token)
{
	std::swap(_args, token._args);
	std::swap(_code, token._code);
}



bool override_object(ObjectToken *, ObjectToken const &)
{
	return false;
}

void print_debug(std::ostream * const out, ObjectToken const & in)
{
	*out << "ObjectToken(";
		*out << "args=(";
		print_debug(out, in._args);
		*out << ")";

		*out << ", ";

		*out << "code=(";
		print_debug(out, in._code);
		*out << ")";

		*out << ", ";

		*out << "labels=(";
		print_debug(out, in._labels);
		*out << ")";

		*out << ", ";

		*out << "position=(";
		print_debug(out, in._position);
		*out << ")";
	*out << ")";
}

void read_object(std::istream * in, ObjectToken * out)
{
	read_object(in, &out->_args);
	read_object(in, &out->_code);
	read_object(in, &out->_labels);
	read_object(in, &out->_position);
}

void write_object(std::ostream * out, ObjectToken const & in)
{
	write_object(out, in._args);
	write_object(out, in._code);
	write_object(out, in._labels);
	write_object(out, in._position);
}


