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

/* SourceExpression/ValueData.cpp
**
** Defines the SourceExpression_ValueData class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_ValueData
//
class SourceExpression_ValueData : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_ValueData,
                                   SourceExpression);

public:
   SourceExpression_ValueData(bool garbage, VariableType const *type,
                              SRCEXP_EXPR_ARGS);

	virtual VariableType const * getType() const;

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   VariableType const *type;
   bool garbage;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_value_data
//
SRCEXP_EXPRVAL_DEFN(t, data)
{
   return new SourceExpression_ValueData(false, type, context, position);
}

//
// SourceExpression::create_value_data_garbage
//
SRCEXP_EXPRVAL_DEFN(t, data_garbage)
{
   return new SourceExpression_ValueData(true, type, context, position);
}

//
// SourceExpression_ValueData::SourceExpression_ValueData
//
SourceExpression_ValueData::
SourceExpression_ValueData(bool _garbage, VariableType const *_type,
                           SRCEXP_EXPR_PARM)
                           : Super(SRCEXP_EXPR_PASS),
                             type(_type), garbage(_garbage)
{
}

//
// SourceExpression_ValueData::getType
//
VariableType const *SourceExpression_ValueData::getType() const
{
   return type;
}

//
// SourceExpression_ValueData::virtual_makeObjects
//
void SourceExpression_ValueData::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	if (dst->type == VariableData::MT_VOID) return;

	bigsint srcSize = type->size(position);

	VariableData::Pointer src = VariableData::create_stack(srcSize);

	make_objects_memcpy_prep(objects, dst, src, position);

	for (bigsint i = srcSize; i--;)
	{
		// FIXME: Should be based on type.
		if (garbage)
			objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0xDEADBEEF));
		else
			objects->addTokenPushZero();
	}

	make_objects_memcpy_post(objects, dst, src, position);
}

// EOF

