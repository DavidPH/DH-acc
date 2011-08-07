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

/* SourceExpression/BinaryLT.cpp
**
** Defines the SourceExpression_BinaryLT class and methods.
*/

#include "BinaryCompare.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"



class SourceExpression_BinaryLT : public SourceExpression_BinaryCompare
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryLT, SourceExpression_BinaryCompare);

public:
	SourceExpression_BinaryLT(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects);

protected:
	virtual void printDebug(std::ostream * out) const;
};



SourceExpression::Pointer SourceExpression::create_binary_lt(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryLT(exprL, exprR, position);
}



SourceExpression_BinaryLT::SourceExpression_BinaryLT(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position) : Super(exprL, exprR, position)
{

}

void SourceExpression_BinaryLT::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	switch (exprL->getType()->type)
	{
	case SourceVariable::VT_CHAR:
	case SourceVariable::VT_FUNCTION:
	case SourceVariable::VT_INT:
	case SourceVariable::VT_LINESPEC:
	case SourceVariable::VT_NATIVE:
	case SourceVariable::VT_POINTER:
	case SourceVariable::VT_REAL:
	case SourceVariable::VT_SCRIPT:
	case SourceVariable::VT_STRING:
		objects->addToken(OCODE_CMPLT);
		break;

	default:
		throw SourceException("invalid VT", position, getName());
	}
}

void SourceExpression_BinaryLT::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryLT(";
	Super::printDebug(out);
	*out << ")";
}



