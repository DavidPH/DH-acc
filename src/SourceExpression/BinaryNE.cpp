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
// SourceExpression handling of "operator !=".
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
// SourceExpression_BinaryNE
//
class SourceExpression_BinaryNE : public SourceExpression_BinaryCompare
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryNE,
                                   SourceExpression_BinaryCompare);

public:
   SourceExpression_BinaryNE(SRCEXP_EXPRBIN_ARGS);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_ne
//
SRCEXP_EXPRBIN_DEFN(ne)
{
   return new SourceExpression_BinaryNE(exprL, exprR, context, position);
}

//
// SourceExpression_BinaryNE::SourceExpression_BinaryNE
//
SourceExpression_BinaryNE::
SourceExpression_BinaryNE(SRCEXP_EXPRBIN_PARM)
                          : Super(SRCEXP_EXPRBIN_PASS)
{
}

//
// SourceExpression_BinaryNE::virtual_makeObjects
//
void SourceExpression_BinaryNE::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   VariableType::Reference type = getType();
   VariableData::Pointer   src  =
      VariableData::create_stack(type->getSize(position));

   switch (exprL->getType()->getBasicType())
   {
   case VariableType::BT_CHAR:
   case VariableType::BT_FUNCTION:
   case VariableType::BT_INT:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_NATIVE:
   case VariableType::BT_POINTER:
   case VariableType::BT_SCRIPT:
   case VariableType::BT_STRING:
   case VariableType::BT_UINT:
      objects->addToken(OCODE_CMP_NE32I);
      break;

   case VariableType::BT_REAL:
      objects->addToken(OCODE_CMP_NE32F);
      break;

   default:
      throw SourceException("invalid BT", position, getName());
   }

   make_objects_memcpy_post(objects, dst, src, type, position);
}

// EOF

