//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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
   CounterPreambleNoClone(SourceExpression_UnarySub, SourceExpression_Unary);

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
// SourceExpression::create_unary_add
//
SRCEXP_EXPRUNA_DEFN(add)
{
   (void)context; (void)pos;
   return expr;
}

//
// SourceExpression::create_unary_sub
//
SRCEXP_EXPRUNA_DEFN(sub)
{
   return new SourceExpression_UnarySub(expr, context, pos);
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
   return ObjectExpression::create_unary_sub(expr->makeObject(), pos);
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
   bigsint                 size = type->getSize(pos);
   VariableData::Pointer   src  = VariableData::create_stack(size);

   switch (bt)
   {
   case VariableType::BT_CHR:
   case VariableType::BT_INT_HH:
   case VariableType::BT_INT_H:
   case VariableType::BT_INT:
   case VariableType::BT_UNS_HH:
   case VariableType::BT_UNS_H:
   case VariableType::BT_UNS:
      objects->addToken(OCODE_NEG_STK_I);
      break;

   case VariableType::BT_FIX_HH:
   case VariableType::BT_FIX_H:
   case VariableType::BT_FIX:
   case VariableType::BT_FIX_L:
   case VariableType::BT_FIX_LL:
      objects->addToken(OCODE_NEG_STK_I);
      break;

   case VariableType::BT_INT_L:
   case VariableType::BT_INT_LL:
   case VariableType::BT_UNS_L:
   case VariableType::BT_UNS_LL:
   {
      std::string label = context->makeLabel();
      std::string labelEnd = label + "_end";

      ObjectExpression::Pointer tmpL = context->getTempVar(0);
      ObjectExpression::Pointer tmpH = context->getTempVar(1);

      objects->addToken(OCODE_INV_STK_I);
      objects->addToken(OCODE_SET_TEMP, tmpH);
      objects->addToken(OCODE_INV_STK_I);
      objects->addToken(OCODE_SET_TEMP, tmpL);

      objects->addToken(OCODE_INC_TEMP_I, tmpL);
      objects->addToken(OCODE_GET_TEMP, tmpL);
      objects->addToken(OCODE_JMP_TRU, objects->getValue(labelEnd));
      objects->addToken(OCODE_INC_TEMP_I, tmpH);

      objects->addLabel(labelEnd);
      objects->addToken(OCODE_GET_TEMP, tmpL);
      objects->addToken(OCODE_GET_TEMP, tmpH);
   }
      break;

   default:
      Error_NP("invalid BT");
   }

   make_objects_memcpy_post(objects, dst, src, type, context, pos);
}

// EOF

