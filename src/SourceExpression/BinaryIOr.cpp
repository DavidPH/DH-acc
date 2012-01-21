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

/* SourceExpression/BinaryIOr.cpp
**
** Defines the SourceExpression_BinaryIOr class and methods.
*/

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryIOr
//
class SourceExpression_BinaryIOr : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryIOr,
                                   SourceExpression_Binary);

public:
   SourceExpression_BinaryIOr(SRCEXP_EXPRBIN_ARGS);

	virtual CounterPointer<ObjectExpression> makeObject() const;

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_ior
//
SRCEXP_EXPRBIN_DEFN(ior)
{
   return new SourceExpression_BinaryIOr(exprL, exprR, context, position);
}

//
// SourceExpression_BinaryIOr::SourceExpression_BinaryIOr
//
SourceExpression_BinaryIOr::
SourceExpression_BinaryIOr(SRCEXP_EXPRBIN_PARM)
                           : Super(false, SRCEXP_EXPRBIN_PASS)
{
}

CounterPointer<ObjectExpression> SourceExpression_BinaryIOr::makeObject() const
{
	return ObjectExpression::create_binary_ior(exprL->makeObject(), exprR->makeObject(), position);
}

void SourceExpression_BinaryIOr::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	switch (getType()->vt)
	{
	case VariableType::VT_CHAR:
	case VariableType::VT_INT:
	case VariableType::VT_POINTER:
		objects->addToken(OCODE_BITWISE_IOR32);
		break;

	default:
		throw SourceException("invalid VT", position, getName());
	}

	make_objects_memcpy_post(objects, dst, VariableData::create_stack(getType()->size(position)), position);
}

// EOF

