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

#include "../object_io.hpp"



class ObjectExpression_ValueFloat : public ObjectExpression
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_ValueFloat, ObjectExpression);

public:
	ObjectExpression_ValueFloat(bigreal value, SourcePosition const & position);
	ObjectExpression_ValueFloat(std::istream * in);

	virtual ExpressionType getType() const;

	virtual void printDebug(std::ostream * out) const;

	virtual bigreal resolveFloat() const;

protected:
	virtual void writeObject(std::ostream * out) const;

private:
	bigreal _value;
};



ObjectExpression::Pointer ObjectExpression::create_value_float(bigreal value, SourcePosition const & position)
{
	return new ObjectExpression_ValueFloat(value, position);
}
ObjectExpression::Pointer ObjectExpression::create_value_float(std::istream * in)
{
	return new ObjectExpression_ValueFloat(in);
}



ObjectExpression_ValueFloat::ObjectExpression_ValueFloat(bigreal value, SourcePosition const & position_) : Super(position_), _value(value)
{

}
ObjectExpression_ValueFloat::ObjectExpression_ValueFloat(std::istream * in) : Super(in)
{
	read_object(in, &_value);
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

bigreal ObjectExpression_ValueFloat::resolveFloat() const
{
	return _value;
}

void ObjectExpression_ValueFloat::writeObject(std::ostream * out) const
{
	write_object(out, OT_VALUE_FLOAT);

	Super::writeObject(out);

	write_object(out, _value);
}


