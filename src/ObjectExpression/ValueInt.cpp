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

#include "../ObjectExpression.hpp"



class ObjectExpression_ValueInt : public ObjectExpression
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_ValueInt, ObjectExpression);

public:
	ObjectExpression_ValueInt(int_t value, SourcePosition const & position);

	virtual ExpressionType getType() const;

	virtual void printDebug(std::ostream * out) const;

	virtual int_t resolveInt() const;

private:
	int_t _value;
};



ObjectExpression::Pointer ObjectExpression::create_value_int(int_t value, SourcePosition const & position)
{
	return new ObjectExpression_ValueInt(value, position);
}



ObjectExpression_ValueInt::ObjectExpression_ValueInt(int_t value, SourcePosition const & position) : Super(position), _value(value)
{

}

ObjectExpression::ExpressionType ObjectExpression_ValueInt::getType() const
{
	return ET_INT;
}

void ObjectExpression_ValueInt::printDebug(std::ostream * out) const
{
	*out << "ObjectExpression_ValueInt(";
	Super::printDebug(out);
	*out << " ";
		*out << "value=(";
		*out << _value;
		*out << ")";
	*out << ")";
}

ObjectExpression::int_t ObjectExpression_ValueInt::resolveInt() const
{
	return _value;
}



