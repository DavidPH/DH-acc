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

/* ObjectExpression/BinaryDiv.cpp
**
** Defines the ObjectExpression_BinaryDiv class and methods.
*/

#include "Binary.hpp"



class ObjectExpression_BinaryDiv : public ObjectExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_BinaryDiv, ObjectExpression_Binary);

public:
	ObjectExpression_BinaryDiv(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	ObjectExpression_BinaryDiv(std::istream * in);

	virtual void printDebug(std::ostream * out) const;

	virtual float_t resolveFloat() const;
	virtual int_t resolveInt() const;

protected:
	virtual void writeObject(std::ostream * out) const;
};



ObjectExpression::Pointer ObjectExpression::create_binary_div(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position)
{
	return new ObjectExpression_BinaryDiv(exprL, exprR, position);
}
ObjectExpression::Pointer ObjectExpression::create_binary_div(std::istream * in)
{
	return new ObjectExpression_BinaryDiv(in);
}



ObjectExpression_BinaryDiv::ObjectExpression_BinaryDiv(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position) : Super(exprL, exprR, position)
{

}
ObjectExpression_BinaryDiv::ObjectExpression_BinaryDiv(std::istream * in) : Super(in)
{

}

void ObjectExpression_BinaryDiv::printDebug(std::ostream * out) const
{
	*out << "ObjectExpression_BinaryDiv(";
	Super::printDebug(out);
	*out << ")";
}

ObjectExpression::float_t ObjectExpression_BinaryDiv::resolveFloat() const
{
	if (getType() == ET_FLOAT) return exprL->resolveFloat() / exprR->resolveFloat();

	return Super::resolveFloat();
}
ObjectExpression::int_t ObjectExpression_BinaryDiv::resolveInt() const
{
	if (getType() == ET_INT) return exprL->resolveInt() / exprR->resolveInt();

	return Super::resolveInt();
}

void ObjectExpression_BinaryDiv::writeObject(std::ostream * out) const
{
	write_object(out, OT_BINARY_DIV);

	Super::writeObject(out);
}



