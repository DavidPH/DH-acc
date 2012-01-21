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

/* SourceExpression/BinaryGE.cpp
**
** Defines the SourceExpression_BinaryGE class and methods.
*/

#include "BinaryCompare.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryGE
//
class SourceExpression_BinaryGE : public SourceExpression_BinaryCompare
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryGE,
                                   SourceExpression_BinaryCompare);

public:
   SourceExpression_BinaryGE(SRCEXP_EXPRBIN_ARGS);

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_ge
//
SRCEXP_EXPRBIN_DEFN(ge)
{
   return new SourceExpression_BinaryGE(exprL, exprR, context, position);
}

//
// SourceExpression_BinaryGE::SourceExpression_BinaryGE
//
SourceExpression_BinaryGE::
SourceExpression_BinaryGE(SRCEXP_EXPRBIN_PARM)
                          : Super(SRCEXP_EXPRBIN_PASS)
{
}

void SourceExpression_BinaryGE::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	switch (exprL->getType()->vt)
	{
	case VariableType::VT_CHAR:
	case VariableType::VT_INT:
		objects->addToken(OCODE_CMP_GE32I);
		break;

	case VariableType::VT_FUNCTION:
	case VariableType::VT_LINESPEC:
	case VariableType::VT_NATIVE:
	case VariableType::VT_POINTER:
	case VariableType::VT_SCRIPT:
	case VariableType::VT_STRING:
		objects->addToken(OCODE_CMP_GE32U);
		break;

	case VariableType::VT_REAL:
		objects->addToken(OCODE_CMP_GE32F);
		break;

	default:
		throw SourceException("invalid VT", position, getName());
	}

	make_objects_memcpy_post(objects, dst, VariableData::create_stack(getType()->size(position)), position);
}

// EOF

