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

/* ObjectVector.cpp
**
** Defines the ObjectVector methods.
*/

#include "ObjectVector.hpp"



void ObjectVector::addLabel(std::string const & label)
{
	_labels.push_back(label);
}
void ObjectVector::addLabel(std::vector<std::string> const & labels)
{
	for (size_t i(0); i < labels.size(); ++i)
		addLabel(labels[i]);
}

void ObjectVector::addToken(ObjectToken const & token)
{
	_tokens.push_back(token);
}
void ObjectVector::addToken(ObjectToken::ObjectCode code)
{
	addToken(code, std::vector<ObjectExpression::Pointer>());
}
void ObjectVector::addToken(ObjectToken::ObjectCode code, std::vector<ObjectExpression::Pointer> const & args)
{
	_tokens.push_back(ObjectToken(code, _position, _labels, args));
	_labels.clear();
}
void ObjectVector::addToken(ObjectToken::ObjectCode code, ObjectExpression * arg0)
{
	std::vector<ObjectExpression::Pointer> args;

	args.push_back(arg0);

	addToken(code, args);
}
void ObjectVector::addToken(ObjectToken::ObjectCode code, ObjectExpression * arg0, ObjectExpression * arg1)
{
	std::vector<ObjectExpression::Pointer> args;

	args.push_back(arg0);
	args.push_back(arg1);

	addToken(code, args);
}
void ObjectVector::addTokenPushZero()
{
	addToken(ObjectToken::OCODE_PUSHNUMBER, getValue(0));
}

ObjectExpression::Pointer ObjectVector::getValue(ObjectExpression::float_t f) const
{
	return ObjectExpression::create_value_float(f, _position);
}
ObjectExpression::Pointer ObjectVector::getValue(ObjectExpression::int_t i) const
{
	return ObjectExpression::create_value_int(i, _position);
}
ObjectExpression::Pointer ObjectVector::getValue(int i) const
{
	return getValue((ObjectExpression::int_t)i);
}
ObjectExpression::Pointer ObjectVector::getValue(ObjectExpression * expr) const
{
	return expr;
}
ObjectExpression::Pointer ObjectVector::getValue(std::string const & label) const
{
	return ObjectExpression::create_value_symbol(label, _position);
}
ObjectExpression::Pointer ObjectVector::getValue(unsigned int i) const
{
	return getValue((ObjectExpression::int_t)i);
}

ObjectToken const & ObjectVector::operator [] (ObjectExpression::int_t index) const
{
	return _tokens[(size_t)index];
}

ObjectExpression::int_t ObjectVector::size() const
{
	return (ObjectExpression::int_t)_tokens.size();
}

ObjectVector & ObjectVector::setPosition(SourcePosition const & position)
{
	_position = position;

	return *this;
}



