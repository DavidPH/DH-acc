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

/* ObjectExpression/BinaryAdd.cpp
**
** Defines the ObjectExpression_BinaryAdd class and methods.
*/

#include "Binary.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"



class ObjectExpression_BinaryAdd : public ObjectExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(ObjectExpression_BinaryAdd, ObjectExpression_Binary);

public:
	ObjectExpression_BinaryAdd(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	ObjectExpression_BinaryAdd(std::istream * in);

	virtual void printDebug(std::ostream * out) const;

	virtual bigreal resolveFloat() const;
	virtual bigsint resolveInt() const;

	virtual void writeACSP(std::ostream * out) const;

protected:
	virtual void writeObject(std::ostream * out) const;
};



ObjectExpression::Pointer ObjectExpression::create_binary_add(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position)
{
	return new ObjectExpression_BinaryAdd(exprL, exprR, position);
}
ObjectExpression::Pointer ObjectExpression::create_binary_add(std::istream * in)
{
	return new ObjectExpression_BinaryAdd(in);
}



ObjectExpression_BinaryAdd::ObjectExpression_BinaryAdd(ObjectExpression * exprL_, ObjectExpression * exprR_, SourcePosition const & position_) : Super(exprL_, exprR_, position_)
{

}
ObjectExpression_BinaryAdd::ObjectExpression_BinaryAdd(std::istream * in) : Super(in)
{

}

void ObjectExpression_BinaryAdd::printDebug(std::ostream * out) const
{
	*out << "ObjectExpression_BinaryAdd(";
	Super::printDebug(out);
	*out << ")";
}

bigreal ObjectExpression_BinaryAdd::resolveFloat() const
{
	if (getType() == ET_FLOAT) return exprL->resolveFloat() + exprR->resolveFloat();

	return Super::resolveFloat();
}
bigsint ObjectExpression_BinaryAdd::resolveInt() const
{
	if (getType() == ET_INT) return exprL->resolveInt() + exprR->resolveInt();

	return Super::resolveInt();
}

void ObjectExpression_BinaryAdd::writeACSP(std::ostream * out) const
{
	BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_ADD);

	exprL->writeACSP(out);
	exprR->writeACSP(out);
}

void ObjectExpression_BinaryAdd::writeObject(std::ostream * out) const
{
	write_object(out, OT_BINARY_ADD);

	Super::writeObject(out);
}


