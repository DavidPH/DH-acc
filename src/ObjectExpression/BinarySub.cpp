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

/* ObjectExpression/BinarySub.cpp
**
** Defines the ObjectExpression_BinarySub class and methods.
*/

#include "Binary.hpp"



class ObjectExpression_BinarySub : public ObjectExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_BinarySub, ObjectExpression_Binary);

public:
	ObjectExpression_BinarySub(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	ObjectExpression_BinarySub(std::istream * in);

	virtual void printDebug(std::ostream * out) const;

	virtual bigreal resolveFloat() const;
	virtual bigsint resolveInt() const;

protected:
	virtual void writeObject(std::ostream * out) const;
};



ObjectExpression::Pointer ObjectExpression::create_binary_sub(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position)
{
	return new ObjectExpression_BinarySub(exprL, exprR, position);
}
ObjectExpression::Pointer ObjectExpression::create_binary_sub(std::istream * in)
{
	return new ObjectExpression_BinarySub(in);
}



ObjectExpression_BinarySub::ObjectExpression_BinarySub(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position) : Super(exprL, exprR, position)
{

}
ObjectExpression_BinarySub::ObjectExpression_BinarySub(std::istream * in) : Super(in)
{

}

void ObjectExpression_BinarySub::printDebug(std::ostream * const out) const
{
	*out << "ObjectExpression_BinarySub(";
	Super::printDebug(out);
	*out << ")";
}

bigreal ObjectExpression_BinarySub::resolveFloat() const
{
	if (getType() == ET_FLOAT) return exprL->resolveFloat() - exprR->resolveFloat();

	return Super::resolveFloat();
}
bigsint ObjectExpression_BinarySub::resolveInt() const
{
	if (getType() == ET_INT) return exprL->resolveInt() - exprR->resolveInt();

	return Super::resolveInt();
}

void ObjectExpression_BinarySub::writeObject(std::ostream * out) const
{
	write_object(out, OT_BINARY_SUB);

	Super::writeObject(out);
}



