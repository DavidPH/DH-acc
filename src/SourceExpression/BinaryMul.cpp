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

/* SourceExpression/BinaryMul.cpp
**
** Defines the SourceExpression_BinaryMul class and methods.
*/

#include "Binary.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"



class SourceExpression_BinaryMul : public SourceExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryMul, SourceExpression_Binary);

public:
	SourceExpression_BinaryMul(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * out) const;
};



SourceExpression::Pointer SourceExpression::create_binary_mul(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryMul(exprL, exprR, position);
}



SourceExpression_BinaryMul::SourceExpression_BinaryMul(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position) : Super(exprL, exprR, true, position)
{

}

void SourceExpression_BinaryMul::makeObjectsGet(ObjectVector * objects) const
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
		objects->addToken(ObjectToken::OCODE_MUL);
		break;

	case SourceVariable::VT_REAL:
		objects->addToken(ObjectToken::OCODE_MULFIXED);
		break;
	}
}

void SourceExpression_BinaryMul::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryMul(";
	Super::printDebug(out);
	*out << ")";
}



