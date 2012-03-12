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
// SourceExpression handling of "operator ~".
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_UnaryNot
//
class SourceExpression_UnaryNot : public SourceExpression_Unary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_UnaryNot,
                                   SourceExpression_Unary);

public:
   SourceExpression_UnaryNot(SRCEXP_EXPRUNA_ARGS);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_unary_not
//
SRCEXP_EXPRUNA_DEFN(not)
{
   return new SourceExpression_UnaryNot(expr, context, position);
}

//
// SourceExpression_UnaryNot::SourceExpression_UnaryNot
//
SourceExpression_UnaryNot::
SourceExpression_UnaryNot(SRCEXP_EXPRUNA_PARM)
                          : Super(NULL, SRCEXP_EXPRUNA_PASS)
{
}

//
// SourceExpression_UnaryNot::virtual_makeObjects
//
void SourceExpression_UnaryNot::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   VariableType::Reference type = getType();
   VariableData::Pointer   src  =
      VariableData::create_stack(type->getSize(position));

   switch (type->getBasicType())
   {
   case VariableType::BT_CHAR:
   case VariableType::BT_INT:
   case VariableType::BT_LONG:
   case VariableType::BT_SCHAR:
   case VariableType::BT_SHORT:
   case VariableType::BT_UCHAR:
   case VariableType::BT_UINT:
   case VariableType::BT_ULONG:
   case VariableType::BT_USHORT:
      objects->addToken(OCODE_BITWISE_NOT32);
      break;

   case VariableType::BT_LLONG:
   case VariableType::BT_ULLONG:
   {
      ObjectExpression::Pointer tmpH = context->getTempVar(0);

      objects->addToken(OCODE_BITWISE_NOT32);
      objects->addToken(OCODE_SET_TEMP, tmpH);
      objects->addToken(OCODE_BITWISE_NOT32);
      objects->addToken(OCODE_GET_TEMP, tmpH);
   }
      break;

   default:
      ERROR_N(position, "invalid BT");
   }

   make_objects_memcpy_post(objects, dst, src, type, context, position);
}

// EOF

