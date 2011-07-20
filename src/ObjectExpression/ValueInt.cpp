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

/* ObjectExpression/ValueInt.cpp
**
** Defines the ObjectExpression_ValueInt class and methods.
*/

#include "Base.hpp"



class ObjectExpression_ValueInt : public ObjectExpression_Base
{
public:
	ObjectExpression_ValueInt(ObjectExpression::int_t value, SourcePosition const & position);

	virtual ObjectExpression_ValueInt * clone() const;

	virtual char const * getName() const;

	virtual ObjectExpression::ExpressionType getType() const;

	virtual void printDebug(std::ostream * out) const;

	virtual ObjectExpression::float_t resolveFloat() const;
	virtual ObjectExpression::int_t resolveInt() const;

private:
	ObjectExpression::int_t _value;
};



ObjectExpression ObjectExpression::create_value_int(ObjectExpression::int_t value, SourcePosition const & position)
{
	return ObjectExpression_ValueInt(value, position);
}



ObjectExpression_ValueInt::ObjectExpression_ValueInt(ObjectExpression::int_t value, SourcePosition const & position) : ObjectExpression_Base(position), _value(value)
{

}

ObjectExpression_ValueInt * ObjectExpression_ValueInt::clone() const
{
	return new ObjectExpression_ValueInt(*this);
}

char const * ObjectExpression_ValueInt::getName() const
{
	return "ObjectExpression_ValueInt";
}

ObjectExpression::ExpressionType ObjectExpression_ValueInt::getType() const
{
	return ObjectExpression::ET_INT;
}

void ObjectExpression_ValueInt::printDebug(std::ostream * const out) const
{
	*out << "ObjectExpression_ValueInt(";
	ObjectExpression_Base::printDebug(out);
	*out << " ";
		*out << "value=(";
		*out << _value;
		*out << ")";
	*out << ")";
}

ObjectExpression::float_t ObjectExpression_ValueInt::resolveFloat() const
{
	return (ObjectExpression::float_t)_value;
}
ObjectExpression::int_t ObjectExpression_ValueInt::resolveInt() const
{
	return _value;
}



