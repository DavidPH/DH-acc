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
// SourceExpression_UnarySub
//
class SourceExpression_UnarySub : public SourceExpression_Unary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_UnarySub,
                                   SourceExpression_Unary);

public:
   SourceExpression_UnarySub(SRCEXP_EXPRUNA_ARGS);

   virtual bool canMakeObject() const;

   virtual CounterPointer<ObjectExpression> makeObject() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_unary_sub
//
SRCEXP_EXPRUNA_DEFN(sub)
{
   return new SourceExpression_UnarySub(expr, context, position);
}

//
// SourceExpression_UnarySub::SourceExpression_UnarySub
//
SourceExpression_UnarySub::
SourceExpression_UnarySub(SRCEXP_EXPRUNA_PARM)
                          : Super(NULL, SRCEXP_EXPRUNA_PASS)
{
}

//
// SourceExpression_UnarySub::canMakeObject
//
bool SourceExpression_UnarySub::canMakeObject() const
{
   return expr->canMakeObject();
}

//
// SourceExpression_UnarySub::makeObject
//
CounterPointer<ObjectExpression> SourceExpression_UnarySub::makeObject() const
{
   return ObjectExpression::create_unary_sub(expr->makeObject(), position);
}

//
// SourceExpression_UnarySub::virtual_makeObjects
//
void SourceExpression_UnarySub::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   VariableType::Reference type = getType();
   VariableType::BasicType bt   = type->getBasicType();
   bigsint                 size = type->getSize(position);
   VariableData::Pointer   src  = VariableData::create_stack(size);

   switch (bt)
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
      objects->addToken(OCODE_MISC_NEGATE32I);
      break;

   case VariableType::BT_FIXED:
   case VariableType::BT_REAL:
      objects->addToken(OCODE_MISC_NEGATE32F);
      break;

   case VariableType::BT_LLONG:
   case VariableType::BT_ULLONG:
   {
      std::string labelEnd = label + "_end";

      ObjectExpression::Pointer tmpL = context->getTempVar(0);
      ObjectExpression::Pointer tmpH = context->getTempVar(1);

      objects->addToken(OCODE_BITWISE_NOT32);
      objects->addToken(OCODE_SET_TEMP, tmpH);
      objects->addToken(OCODE_BITWISE_NOT32);
      objects->addToken(OCODE_SET_TEMP, tmpL);

      objects->addToken(OCODE_SETOP_INC_TEMP, tmpL);
      objects->addToken(OCODE_GET_TEMP, tmpL);
      objects->addToken(OCODE_BRANCH_TRUE, objects->getValue(labelEnd));
      objects->addToken(OCODE_SETOP_INC_TEMP, tmpH);

      objects->addLabel(labelEnd);
      objects->addToken(OCODE_GET_TEMP, tmpL);
      objects->addToken(OCODE_GET_TEMP, tmpH);
   }
      break;

   default:
      ERROR_N(position, "invalid BT");
   }

   make_objects_memcpy_post(objects, dst, src, type, context, position);
}

// EOF

