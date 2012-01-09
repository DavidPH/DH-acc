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

/* SourceExpression/UnarySub.cpp
**
** Defines the SourceExpression_UnarySub class and methods.
*/

#include "Unary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



class SourceExpression_UnarySub : public SourceExpression_Unary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_UnarySub, SourceExpression_Unary);

public:
	SourceExpression_UnarySub(SourceExpression * expr, SourcePosition const & position);

	virtual bool canMakeObject() const;

	virtual CounterPointer<ObjectExpression> makeObject() const;

protected:
	virtual void printDebug(std::ostream * const out) const;

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};



SourceExpression::Pointer SourceExpression::create_unary_sub(SourceExpression * expr, SourcePosition const & position)
{
	return new SourceExpression_UnarySub(expr, position);
}



SourceExpression_UnarySub::SourceExpression_UnarySub(SourceExpression * expr_, SourcePosition const & position_) : Super(expr_, NULL, position_)
{

}

bool SourceExpression_UnarySub::canMakeObject() const
{
	return expr->canMakeObject();
}

CounterPointer<ObjectExpression> SourceExpression_UnarySub::makeObject() const
{
	return ObjectExpression::create_unary_sub(expr->makeObject(), position);
}

void SourceExpression_UnarySub::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_UnarySub(";
	Super::printDebug(out);
	*out << ")";
}

void SourceExpression_UnarySub::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	switch (getType()->vt)
	{
	case VariableType::VT_CHAR:
	case VariableType::VT_INT:
	case VariableType::VT_POINTER:
		objects->addToken(OCODE_MISC_NEGATE32I);
		break;

	case VariableType::VT_REAL:
		objects->addToken(OCODE_MISC_NEGATE32F);
		break;

	default:
		throw SourceException("invalid VT", position, getName());
	}

	make_objects_memcpy_post(objects, dst, VariableData::create_stack(getType()->size(position)), position);
}


