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

/* SourceExpression/UnaryDereference.cpp
**
** Defines the SourceExpression_UnaryDereference class and methods.
*/

#include "Unary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../ost_type.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"



class SourceExpression_UnaryDereference : public SourceExpression_Unary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_UnaryDereference, SourceExpression_Unary);

public:
	SourceExpression_UnaryDereference(SourceExpression * expr, SourcePosition const & position);

	virtual bool canGetData() const;

	virtual bool canMakeObjectAddress() const;

	virtual bool canMakeObjectsAddress() const;

	virtual VariableData::Pointer getData() const;

	virtual VariableType const * getType() const;

	virtual CounterPointer<ObjectExpression> makeObjectAddress() const;

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

	virtual void virtual_makeObjectsAddress(ObjectVector *objects, VariableData *dst);
};



SourceExpression::Pointer SourceExpression::create_unary_dereference(SourceExpression * expr, SourcePosition const & position)
{
	return new SourceExpression_UnaryDereference(expr, position);
}

SourceExpression_UnaryDereference::SourceExpression_UnaryDereference(SourceExpression *_expr, SourcePosition const &_position) : Super(_expr, _position)
{
}

//
// SourceExpression_UnaryDereference::canGetData
//
bool SourceExpression_UnaryDereference::canGetData() const
{
   return getType()->vt != VariableType::VT_STRING;
}

bool SourceExpression_UnaryDereference::canMakeObjectAddress() const
{
	return expr->canMakeObject();
}

bool SourceExpression_UnaryDereference::canMakeObjectsAddress() const
{
	return true;
}

//
// SourceExpression_UnaryDereference::getData
//
VariableData::Pointer SourceExpression_UnaryDereference::getData() const
{
   return VariableData::
      create_pointer(getType()->size(position),
                     ObjectExpression::create_value_int(0, position), expr);
}

VariableType const * SourceExpression_UnaryDereference::getType() const
{
	return expr->getType()->refType;
}

CounterPointer<ObjectExpression> SourceExpression_UnaryDereference::makeObjectAddress() const
{
	return expr->makeObject();
}

void SourceExpression_UnaryDereference::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	if (expr->getType()->vt == VariableType::VT_STRING)
	{
		VariableData::Pointer tmp = VariableData::create_stack(getType()->size(position));

		make_objects_memcpy_prep(objects, dst, tmp, position);

		expr->makeObjects(objects, VariableData::create_stack(expr->getType()->size(position)));
		objects->addTokenPushZero();
		objects->addToken(OCODE_MISC_NATIVE, objects->getValue(2), objects->getValue(15));

		make_objects_memcpy_post(objects, dst, tmp, position);

		return;
	}

	VariableData::Pointer src = getData();

	make_objects_memcpy_prep(objects, dst, src, position);
	make_objects_memcpy_post(objects, dst, src, position);
}

void SourceExpression_UnaryDereference::virtual_makeObjectsAddress(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjectsAddress(objects, dst);

	expr->makeObjects(objects, dst);
}

// EOF

