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
// SourceExpression handling of "operator -".
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
// SourceExpression_BinarySub
//
class SourceExpression_BinarySub : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinarySub,
                                   SourceExpression_Binary);

public:
   SourceExpression_BinarySub(SRCEXP_EXPRBIN_ARGS);

   virtual CounterPointer<ObjectExpression> makeObject() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_sub
//
SRCEXP_EXPRBIN_DEFN(sub)
{
   return new SourceExpression_BinarySub(exprL, exprR, context, position);
}

//
// SourceExpression_BinarySub::SourceExpression_BinarySub
//
SourceExpression_BinarySub::
SourceExpression_BinarySub(SRCEXP_EXPRBIN_PARM)
                           : Super(true, SRCEXP_EXPRBIN_PASS)
{
}

CounterPointer<ObjectExpression> SourceExpression_BinarySub::makeObject() const
{
   return ObjectExpression::create_binary_sub
          (exprL->makeObject(), exprR->makeObject(), position);
}

//
// SourceExpression_BinarySub::virtual_makeObjects
//
void SourceExpression_BinarySub::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   switch (getType()->getBasicType())
   {
   case VariableType::BT_CHAR:
   case VariableType::BT_INT:
      objects->addToken(OCODE_SUB32I);
      break;

   case VariableType::BT_POINTER:
   case VariableType::BT_UINT:
      objects->addToken(OCODE_SUB32U);
      break;

   case VariableType::BT_REAL:
      objects->addToken(OCODE_SUB32F);
      break;

   default:
      throw SourceException("invalid BT", position, getName());
   }

   make_objects_memcpy_post
   (objects, dst, VariableData::create_stack(getType()->getSize(position)), position);
}

// EOF

