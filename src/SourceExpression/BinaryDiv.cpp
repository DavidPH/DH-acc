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

/* SourceExpression/BinaryDiv.cpp
**
** Defines the SourceExpression_BinaryDiv class and methods.
*/

#include "Binary.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"



class SourceExpression_BinaryDiv : public SourceExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryDiv, SourceExpression_Binary);

public:
	SourceExpression_BinaryDiv(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * out) const;
};



SourceExpression::Pointer SourceExpression::create_binary_div(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryDiv(exprL, exprR, position);
}



SourceExpression_BinaryDiv::SourceExpression_BinaryDiv(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position) : Super(exprL, exprR, true, position)
{

}

void SourceExpression_BinaryDiv::makeObjectsGet(ObjectVector * objects) const
{
	Super::makeObjectsGet(objects);

	objects->setPosition(position);

	switch (getType()->type)
	{
	case SourceVariable::VT_ARRAY:
	case SourceVariable::VT_ASMFUNC:
	case SourceVariable::VT_BLOCK:
	case SourceVariable::VT_FUNCTION:
	case SourceVariable::VT_LINESPEC:
	case SourceVariable::VT_NATIVE:
	case SourceVariable::VT_SCRIPT:
	case SourceVariable::VT_STRING:
	case SourceVariable::VT_STRUCT:
	case SourceVariable::VT_VOID:
		throw SourceException("invalid VT", position, getName());

	case SourceVariable::VT_CHAR:
	case SourceVariable::VT_INT:
	case SourceVariable::VT_POINTER:
		objects->addToken(ObjectToken::OCODE_DIV);
		break;

	case SourceVariable::VT_REAL:
		objects->addToken(ObjectToken::OCODE_DIVFIXED);
		break;
	}
}

void SourceExpression_BinaryDiv::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryDiv(";
	Super::printDebug(out);
	*out << ")";
}



