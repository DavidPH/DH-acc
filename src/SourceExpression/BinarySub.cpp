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

/* SourceExpression/BinarySub.cpp
**
** Defines the SourceExpression_BinarySub class and methods.
*/

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



class SourceExpression_BinarySub : public SourceExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinarySub, SourceExpression_Binary);

public:
	SourceExpression_BinarySub(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual CounterPointer<ObjectExpression> makeObject() const;

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	virtual void virtual_makeObjectsGet(ObjectVector * objects);
};



SourceExpression::Pointer SourceExpression::create_binary_sub(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinarySub(exprL, exprR, position);
}



SourceExpression_BinarySub::SourceExpression_BinarySub(SourceExpression * exprL_, SourceExpression * exprR_, SourcePosition const & position_) : Super(exprL_, exprR_, true, position_)
{

}

CounterPointer<ObjectExpression> SourceExpression_BinarySub::makeObject() const
{
	return ObjectExpression::create_binary_sub(exprL->makeObject(), exprR->makeObject(), position);
}

void SourceExpression_BinarySub::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinarySub(";
	Super::printDebug(out);
	*out << ")";
}

void SourceExpression_BinarySub::virtual_makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	switch (getType()->vt)
	{
	case VariableType::VT_CHAR:
	case VariableType::VT_INT:
	case VariableType::VT_POINTER:
	case VariableType::VT_REAL:
		objects->addToken(OCODE_SUB);
		break;

	default:
		throw SourceException("invalid VT", position, getName());
	}
}


