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

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



class SourceExpression_BinaryDiv : public SourceExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryDiv, SourceExpression_Binary);

public:
	SourceExpression_BinaryDiv(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual CounterPointer<ObjectExpression> makeObject() const;

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};



SourceExpression::Pointer SourceExpression::create_binary_div(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryDiv(exprL, exprR, position);
}



SourceExpression_BinaryDiv::SourceExpression_BinaryDiv(SourceExpression * exprL_, SourceExpression * exprR_, SourcePosition const & position_) : Super(exprL_, exprR_, true, position_)
{

}

CounterPointer<ObjectExpression> SourceExpression_BinaryDiv::makeObject() const
{
	return ObjectExpression::create_binary_div(exprL->makeObject(), exprR->makeObject(), position);
}

void SourceExpression_BinaryDiv::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryDiv(";
	Super::printDebug(out);
	*out << ")";
}

void SourceExpression_BinaryDiv::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	switch (getType()->vt)
	{
	case VariableType::VT_CHAR:
	case VariableType::VT_INT:
		objects->addToken(OCODE_DIV32I);
		break;

	case VariableType::VT_POINTER:
		objects->addToken(OCODE_DIV32U);
		break;

	case VariableType::VT_REAL:
		objects->addToken(OCODE_DIV32F);
		break;

	default:
		throw SourceException("invalid VT", position, getName());
	}

	make_objects_memcpy_post(objects, dst, VariableData::create_stack(getType()->size(position)), position);
}


