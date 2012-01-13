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

/* SourceExpression/UnaryReference.cpp
**
** Defines the SourceExpression_UnaryReference class and methods.
*/

#include "Unary.hpp"

#include "../VariableType.hpp"



class SourceExpression_UnaryReference : public SourceExpression_Unary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_UnaryReference, SourceExpression_Unary);

public:
	SourceExpression_UnaryReference(SourceExpression * expr, SourcePosition const & position);

	virtual VariableType const * getType() const;

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

	VariableType const * _type;
};



SourceExpression::Pointer SourceExpression::create_unary_reference(SourceExpression * expr, SourcePosition const & position)
{
	return new SourceExpression_UnaryReference(expr, position);
}

SourceExpression_UnaryReference::SourceExpression_UnaryReference(SourceExpression * expr_, SourcePosition const & position_) : Super(expr_, position_), _type(VariableType::get_pointer(expr->getType()))
{
}

VariableType const * SourceExpression_UnaryReference::getType() const
{
	return _type;
}

void SourceExpression_UnaryReference::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	expr->makeObjectsAddress(objects, dst);
}

// EOF

