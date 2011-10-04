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

/* ObjectExpression/ValueOCode.cpp
**
** Defines the ObjectExpression_ValueOCode class and methods.
*/

#include "../ObjectExpression.hpp"

#include "../ObjectCode.hpp"



class ObjectExpression_ValueOCode : public ObjectExpression
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_ValueOCode, ObjectExpression);

public:
	ObjectExpression_ValueOCode(ObjectCodeSet const & value, SourcePosition const & position);
	ObjectExpression_ValueOCode(std::istream * in);

	virtual ExpressionType getType() const;

	virtual void printDebug(std::ostream * out) const;

	virtual ObjectCodeSet resolveOCode() const;

protected:
	virtual void writeObject(std::ostream * out) const;

private:
	ObjectCodeSet _value;
};



ObjectExpression::Pointer ObjectExpression::create_value_ocode(ObjectCodeSet const & value, SourcePosition const & position)
{
	return new ObjectExpression_ValueOCode(value, position);
}
ObjectExpression::Pointer ObjectExpression::create_value_ocode(std::istream * in)
{
	return new ObjectExpression_ValueOCode(in);
}



ObjectExpression_ValueOCode::ObjectExpression_ValueOCode(ObjectCodeSet const & value, SourcePosition const & position_) : Super(position_), _value(value)
{

}
ObjectExpression_ValueOCode::ObjectExpression_ValueOCode(std::istream * in) : Super(in)
{
	read_object(in, &_value);
}

ObjectExpression::ExpressionType ObjectExpression_ValueOCode::getType() const
{
	return ET_OCODE;
}

void ObjectExpression_ValueOCode::printDebug(std::ostream * out) const
{
	*out << "ObjectExpression_ValueOCode(";
	Super::printDebug(out);
	*out << " ";
		*out << "value=(";
		print_debug(out, _value);
		*out << ")";
	*out << ")";
}

ObjectCodeSet ObjectExpression_ValueOCode::resolveOCode() const
{
	return _value;
}

void ObjectExpression_ValueOCode::writeObject(std::ostream * out) const
{
	write_object(out, OT_VALUE_OCODE);

	Super::writeObject(out);

	write_object(out, _value);
}


