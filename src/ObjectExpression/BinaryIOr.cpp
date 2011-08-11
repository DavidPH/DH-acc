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

/* ObjectExpression/BinaryIOr.cpp
**
** Defines the ObjectExpression_BinaryIOr class and methods.
*/

#include "Binary.hpp"



class ObjectExpression_BinaryIOr : public ObjectExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_BinaryIOr, ObjectExpression_Binary);

public:
	ObjectExpression_BinaryIOr(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	ObjectExpression_BinaryIOr(std::istream * in);

	virtual void printDebug(std::ostream * out) const;

	virtual int_t resolveInt() const;

protected:
	virtual void writeObject(std::ostream * out) const;
};



ObjectExpression::Pointer ObjectExpression::create_binary_ior(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position)
{
	return new ObjectExpression_BinaryIOr(exprL, exprR, position);
}
ObjectExpression::Pointer ObjectExpression::create_binary_ior(std::istream * in)
{
	return new ObjectExpression_BinaryIOr(in);
}



ObjectExpression_BinaryIOr::ObjectExpression_BinaryIOr(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position) : Super(exprL, exprR, position)
{

}
ObjectExpression_BinaryIOr::ObjectExpression_BinaryIOr(std::istream * in) : Super(in)
{

}

void ObjectExpression_BinaryIOr::printDebug(std::ostream * out) const
{
	*out << "ObjectExpression_BinaryIOr(";
	Super::printDebug(out);
	*out << ")";
}

ObjectExpression::int_t ObjectExpression_BinaryIOr::resolveInt() const
{
	if (getType() == ET_INT) return exprL->resolveInt() | exprR->resolveInt();

	return Super::resolveInt();
}

void ObjectExpression_BinaryIOr::writeObject(std::ostream * out) const
{
	write_object(out, OT_BINARY_IOR);

	Super::writeObject(out);
}



