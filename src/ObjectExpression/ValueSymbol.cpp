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

/* ObjectExpression/ValueSymbol.cpp
**
** Defines the ObjectExpression_ValueSymbol class and methods.
*/

#include "Base.hpp"

#include "../print_debug.hpp"



class ObjectExpression_ValueSymbol : public ObjectExpression_Base
{
public:
	ObjectExpression_ValueSymbol(std::string const & value, SourcePosition const & position);

	virtual ObjectExpression_ValueSymbol * clone() const;

	virtual char const * getName() const;

	virtual ObjectExpression::ExpressionType getType() const;

	virtual void printDebug(std::ostream * out) const;

	virtual ObjectExpression::float_t resolveFloat() const;
	virtual ObjectExpression::int_t resolveInt() const;

private:
	std::string _value;
};



ObjectExpression ObjectExpression::create_value_symbol(std::string const & value, SourcePosition const & position)
{
	return ObjectExpression_ValueSymbol(value, position);
}



ObjectExpression_ValueSymbol::ObjectExpression_ValueSymbol(std::string const & value, SourcePosition const & position) : ObjectExpression_Base(position), _value(value)
{

}

ObjectExpression_ValueSymbol * ObjectExpression_ValueSymbol::clone() const
{
	return new ObjectExpression_ValueSymbol(*this);
}

char const * ObjectExpression_ValueSymbol::getName() const
{
	return "ObjectExpression_ValueSymbol";
}

ObjectExpression::ExpressionType ObjectExpression_ValueSymbol::getType() const
{
	return ObjectExpression::get_symbol(_value, getPosition()).getType();
}

void ObjectExpression_ValueSymbol::printDebug(std::ostream * const out) const
{
	*out << "ObjectExpression_ValueSymbol(";
	ObjectExpression_Base::printDebug(out);
	*out << " ";
		*out << "value=(";
		print_debug(out, _value);
		*out << ")";
	*out << ")";
}

ObjectExpression::float_t ObjectExpression_ValueSymbol::resolveFloat() const
{
	return ObjectExpression::get_symbol(_value, getPosition()).resolveFloat();
}
ObjectExpression::int_t ObjectExpression_ValueSymbol::resolveInt() const
{
	return ObjectExpression::get_symbol(_value, getPosition()).resolveInt();
}



