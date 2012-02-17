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
// SourceExpression handling of "operator >".
//
//-----------------------------------------------------------------------------

#include "BinaryCompare.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryGT
//
class SourceExpression_BinaryGT : public SourceExpression_BinaryCompare
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryGT,
                                   SourceExpression_BinaryCompare);

public:
   SourceExpression_BinaryGT(SRCEXP_EXPRBIN_ARGS);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_gt
//
SRCEXP_EXPRBIN_DEFN(gt)
{
   return new SourceExpression_BinaryGT(exprL, exprR, context, position);
}

//
// SourceExpression_BinaryGT::SourceExpression_BinaryGT
//
SourceExpression_BinaryGT::
SourceExpression_BinaryGT(SRCEXP_EXPRBIN_PARM)
                          : Super(SRCEXP_EXPRBIN_PASS)
{
}

//
// SourceExpression_BinaryGT::virtual_makeObjects
//
void SourceExpression_BinaryGT::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   switch (exprL->getType()->getBasicType())
   {
   case VariableType::BT_CHAR:
   case VariableType::BT_INT:
      objects->addToken(OCODE_CMP_GT32I);
      break;

   case VariableType::BT_FUNCTION:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_NATIVE:
   case VariableType::BT_POINTER:
   case VariableType::BT_SCRIPT:
   case VariableType::BT_STRING:
   case VariableType::BT_UINT:
      objects->addToken(OCODE_CMP_GT32U);
      break;

   case VariableType::BT_REAL:
      objects->addToken(OCODE_CMP_GT32F);
      break;

   default:
      throw SourceException("invalid BT", position, getName());
   }

   make_objects_memcpy_post
   (objects, dst, VariableData::create_stack(getType()->getSize(position)), position);
}

// EOF

