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

/* ObjectExpression/ValueFloat.cpp
**
** Defines the ObjectExpression_ValueFloat class and methods.
*/

#include "../ObjectExpression.hpp"



class ObjectExpression_ValueFloat : public ObjectExpression
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_ValueFloat, ObjectExpression);

public:
	ObjectExpression_ValueFloat(float_t value, SourcePosition const & position);

	virtual ExpressionType getType() const;

	virtual void printDebug(std::ostream * out) const;

	virtual float_t resolveFloat() const;

private:
	float_t _value;
};



ObjectExpression::Pointer ObjectExpression::create_value_float(float_t value, SourcePosition const & position)
{
	return new ObjectExpression_ValueFloat(value, position);
}



ObjectExpression_ValueFloat::ObjectExpression_ValueFloat(float_t value, SourcePosition const & position) : Super(position), _value(value)
{

}

ObjectExpression::ExpressionType ObjectExpression_ValueFloat::getType() const
{
	return ET_FLOAT;
}

void ObjectExpression_ValueFloat::printDebug(std::ostream * out) const
{
	*out << "ObjectExpression_ValueFloat(";
	Super::printDebug(out);
	*out << " ";
		*out << "value=(";
		*out << _value;
		*out << ")";
	*out << ")";
}

ObjectExpression::float_t ObjectExpression_ValueFloat::resolveFloat() const
{
	return _value;
}



