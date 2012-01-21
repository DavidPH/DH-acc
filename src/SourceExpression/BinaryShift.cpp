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

/* SourceExpression/BinaryShift.cpp
**
** Defines the SourceExpression_BinaryShift class and methods.
*/

#include "Binary.hpp"

#include "../ObjectVector.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryShift
//
class SourceExpression_BinaryShift : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryShift,
                                   SourceExpression_Binary);

public:
   SourceExpression_BinaryShift(bool right, SRCEXP_EXPRBIN_ARGS);

	virtual bool canMakeObject() const;

private:
	virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

	bool _right;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_shiftl
//
SRCEXP_EXPRBIN_DEFN(shiftl)
{
   return new SourceExpression_BinaryShift
              (false, exprL, exprR, context, position);
}

//
// SourceExpression::create_binary_shiftr
//
SRCEXP_EXPRBIN_DEFN(shiftr)
{
   return new SourceExpression_BinaryShift
              (true, exprL, exprR, context, position);
}

//
// SourceExpression_BinaryShift::SourceExpression_BinaryShift
//
SourceExpression_BinaryShift::
SourceExpression_BinaryShift(bool right, SRCEXP_EXPRBIN_PARM)
                             : Super(VariableType::get_vt_int(),
                                     VariableType::get_vt_int(),
                                     false, SRCEXP_EXPRBIN_PASS),
                               _right(right)
{
}

bool SourceExpression_BinaryShift::canMakeObject() const
{
	return false;
}

void SourceExpression_BinaryShift::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
	Super::recurse_makeObjects(objects, dst);

	objects->addToken(_right ? OCODE_BITWISE_SHIFTR32 : OCODE_BITWISE_SHIFTL32);

	make_objects_memcpy_post(objects, dst, VariableData::create_stack(getType()->size(position)), position);
}

// EOF

