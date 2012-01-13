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

/* SourceExpression/BinaryAnd.cpp
**
** Defines the SourceExpression_BinaryAnd class and methods.
*/

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



class SourceExpression_BinaryAnd : public SourceExpression_Binary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryAnd, SourceExpression_Binary);

public:
	SourceExpression_BinaryAnd(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual CounterPointer<ObjectExpression> makeObject() const;

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};



SourceExpression::Pointer SourceExpression::create_binary_and(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryAnd(exprL, exprR, position);
}

SourceExpression_BinaryAnd::SourceExpression_BinaryAnd(SourceExpression * exprL_, SourceExpression * exprR_, SourcePosition const & position_) : Super(exprL_, exprR_, false, position_)
{
}

CounterPointer<ObjectExpression> SourceExpression_BinaryAnd::makeObject() const
{
	return ObjectExpression::create_binary_and(exprL->makeObject(), exprR->makeObject(), position);
}

void SourceExpression_BinaryAnd::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	switch (getType()->vt)
	{
	case VariableType::VT_CHAR:
	case VariableType::VT_INT:
	case VariableType::VT_POINTER:
		objects->addToken(OCODE_BITWISE_AND32);
		break;

	default:
		throw SourceException("invalid VT", position, getName());
	}

	make_objects_memcpy_post(objects, dst, VariableData::create_stack(getType()->size(position)), position);
}

// EOF

