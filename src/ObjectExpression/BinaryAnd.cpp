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

/* ObjectExpression/BinaryAnd.cpp
**
** Defines the ObjectExpression_BinaryAnd class and methods.
*/

#include "Binary.hpp"



class ObjectExpression_BinaryAnd : public ObjectExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_BinaryAnd, ObjectExpression_Binary);

public:
	ObjectExpression_BinaryAnd(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	ObjectExpression_BinaryAnd(std::istream * in);

	virtual void printDebug(std::ostream * out) const;

	virtual bigsint resolveInt() const;

protected:
	virtual void writeObject(std::ostream * out) const;
};



ObjectExpression::Pointer ObjectExpression::create_binary_and(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position)
{
	return new ObjectExpression_BinaryAnd(exprL, exprR, position);
}
ObjectExpression::Pointer ObjectExpression::create_binary_and(std::istream * in)
{
	return new ObjectExpression_BinaryAnd(in);
}



ObjectExpression_BinaryAnd::ObjectExpression_BinaryAnd(ObjectExpression * exprL_, ObjectExpression * exprR_, SourcePosition const & position_) : Super(exprL_, exprR_, position_)
{

}
ObjectExpression_BinaryAnd::ObjectExpression_BinaryAnd(std::istream * in) : Super(in)
{

}

void ObjectExpression_BinaryAnd::printDebug(std::ostream * out) const
{
	*out << "ObjectExpression_BinaryAnd(";
	Super::printDebug(out);
	*out << ")";
}

bigsint ObjectExpression_BinaryAnd::resolveInt() const
{
	if (getType() == ET_INT) return exprL->resolveInt() & exprR->resolveInt();

	return Super::resolveInt();
}

void ObjectExpression_BinaryAnd::writeObject(std::ostream * out) const
{
	write_object(out, OT_BINARY_AND);

	Super::writeObject(out);
}


