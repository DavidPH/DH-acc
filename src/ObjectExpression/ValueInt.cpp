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

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"
#include "../object_io.hpp"



class ObjectExpression_ValueInt : public ObjectExpression
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_ValueInt, ObjectExpression);

public:
	ObjectExpression_ValueInt(bigsint value, SourcePosition const & position);
	ObjectExpression_ValueInt(std::istream * in);

	virtual ExpressionType getType() const;

	virtual bigsint resolveInt() const;

	virtual void writeACSP(std::ostream * out) const;

protected:
	virtual void writeObject(std::ostream * out) const;

private:
	bigsint _value;
};



ObjectExpression::Pointer ObjectExpression::create_value_int(bigsint value, SourcePosition const & position)
{
	return new ObjectExpression_ValueInt(value, position);
}

ObjectExpression::Pointer ObjectExpression::create_value_int(std::istream * in)
{
	return new ObjectExpression_ValueInt(in);
}

ObjectExpression_ValueInt::ObjectExpression_ValueInt(bigsint value, SourcePosition const & position_) : Super(position_), _value(value)
{
}

ObjectExpression_ValueInt::ObjectExpression_ValueInt(std::istream * in) : Super(in)
{
	read_object(in, &_value);
}

ObjectExpression::ExpressionType ObjectExpression_ValueInt::getType() const
{
	return ET_INT;
}

bigsint ObjectExpression_ValueInt::resolveInt() const
{
	return _value;
}

void ObjectExpression_ValueInt::writeACSP(std::ostream * out) const
{
	BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_LITERAL);
	BinaryTokenACS::write_ACS0_32(out, _value);
}

void ObjectExpression_ValueInt::writeObject(std::ostream * out) const
{
	write_object(out, OT_VALUE_INT);

	Super::writeObject(out);

	write_object(out, _value);
}

// EOF

