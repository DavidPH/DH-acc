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

/* ObjectExpression/BinaryXOr.cpp
**
** Defines the ObjectExpression_BinaryXOr class and methods.
*/

#include "Binary.hpp"



class ObjectExpression_BinaryXOr : public ObjectExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_BinaryXOr, ObjectExpression_Binary);

public:
	ObjectExpression_BinaryXOr(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);

	virtual void printDebug(std::ostream * out) const;

	virtual int_t resolveInt() const;
};



ObjectExpression::Pointer ObjectExpression::create_binary_xor(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position)
{
	return new ObjectExpression_BinaryXOr(exprL, exprR, position);
}



ObjectExpression_BinaryXOr::ObjectExpression_BinaryXOr(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position) : Super(exprL, exprR, position)
{

}

void ObjectExpression_BinaryXOr::printDebug(std::ostream * const out) const
{
	*out << "ObjectExpression_BinaryXOr(";
	Super::printDebug(out);
	*out << ")";
}

ObjectExpression::int_t ObjectExpression_BinaryXOr::resolveInt() const
{
	if (getType() == ET_INT) return exprL->resolveInt() ^ exprR->resolveInt();

	return Super::resolveInt();
}



