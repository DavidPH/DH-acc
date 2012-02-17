//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
// SourceExpression handling of "operator /".
//
//-----------------------------------------------------------------------------

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
// SourceExpression_BinaryDiv
//
class SourceExpression_BinaryDiv : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryDiv,
                                   SourceExpression_Binary);

public:
   SourceExpression_BinaryDiv(SRCEXP_EXPRBIN_ARGS);

   virtual CounterPointer<ObjectExpression> makeObject() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_div
//
SRCEXP_EXPRBIN_DEFN(div)
{
   return new SourceExpression_BinaryDiv(exprL, exprR, context, position);
}

//
// SourceExpression_BinaryDiv::SourceExpression_BinaryDiv
//
SourceExpression_BinaryDiv::
SourceExpression_BinaryDiv(SRCEXP_EXPRBIN_PARM)
                           : Super(true, SRCEXP_EXPRBIN_PASS)
{
}

//
// SourceExpression_BinaryDiv::makeObject
//
CounterPointer<ObjectExpression> SourceExpression_BinaryDiv::makeObject() const
{
   return ObjectExpression::create_binary_div
          (exprL->makeObject(), exprR->makeObject(), position);
}

//
// SourceExpression_BinaryDiv::virtual_makeObjects
//
void SourceExpression_BinaryDiv::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   switch (getType()->getBasicType())
   {
   case VariableType::BT_CHAR:
   case VariableType::BT_INT:
      objects->addToken(OCODE_DIV32I);
      break;

   case VariableType::BT_UINT:
      objects->addToken(OCODE_DIV32U);
      break;

   case VariableType::BT_REAL:
      objects->addToken(OCODE_DIV32F);
      break;

   default:
      throw SourceException("invalid BT", position, getName());
   }

   make_objects_memcpy_post
   (objects, dst, VariableData::create_stack(getType()->getSize(position)), position);
}

// EOF

