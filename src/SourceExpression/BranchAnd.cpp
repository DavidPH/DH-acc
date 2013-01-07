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
// SourceExpression handling of "operator &&" and "operator &&=".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BranchAnd
//
class SourceExpression_BranchAnd : public SourceExpression_Binary
{
   CounterPreambleNoClone(SourceExpression_BranchAnd, SourceExpression_Binary);

public:
   SourceExpression_BranchAnd(SRCEXP_EXPRBIN_PARM) : Super(SRCEXP_EXPRBIN_PASS,
      VariableType::get_bt_bit_sft(), VariableType::get_bt_bit_sft(), false) {}

   VariableType::Reference getType() const {return VariableType::get_bt_bit_hrd();}

   //
   // makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      return ObjectExpression::create_branch_and(exprL->makeObject(), exprR->makeObject(), pos);
   }

private:
   //
   // virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      auto    srcType = VariableType::get_bt_bit_hrd();
      bigsint srcSize = srcType->getSize(pos);

      auto src = VariableData::create_stack(srcSize);
      auto tmp = VariableData::create_stack(srcSize);

      std::string label    = context->makeLabel();
      std::string label0   = label + "_0";
      std::string labelEnd = label + "_end";

      // If the right expression is safe, then we don't need to short-circuit
      // around its evaluation.
      if(exprR->isSafe())
      {
         if(dst->type == VariableData::MT_VOID)
         {
            exprL->makeObjects(objects);
            exprR->makeObjects(objects); // Technically unnecessary.
         }
         else
         {
            exprL->makeObjects(objects, tmp);
            exprR->makeObjects(objects, tmp);
            objects->setPosition(pos);
            objects->addToken(OCODE_LOGAND_STK_I);
         }
      }
      else if(dst->type == VariableData::MT_VOID)
      {
         exprL->makeObjects(objects, tmp);
         objects->setPosition(pos);
         objects->addToken(OCODE_JMP_NIL, objects->getValue(labelEnd));

         exprR->makeObjects(objects, dst);

         objects->addLabel(labelEnd);
      }
      else
      {
         make_objects_memcpy_prep(objects, dst, src, pos);

         exprL->makeObjects(objects, tmp);
         objects->setPosition(pos);
         objects->addToken(OCODE_JMP_NIL, objects->getValue(label0));

         exprR->makeObjects(objects, tmp);
         objects->setPosition(pos);
         objects->addToken(OCODE_JMP_NIL, objects->getValue(label0));

         objects->addToken(OCODE_GET_IMM, objects->getValue(1));
         objects->addToken(OCODE_JMP_IMM, objects->getValue(labelEnd));

         objects->addLabel(label0);
         objects->addToken(OCODE_GET_IMM, objects->getValue(0));

         objects->addLabel(labelEnd);

         make_objects_memcpy_post(objects, dst, src, srcType, context, pos);
      }
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_and
//
SRCEXP_EXPRBRA_DEFN(b, and)
{
   return new SourceExpression_BranchAnd(exprL, exprR, context, pos);
}

//
// SourceExpression::create_branch_and_eq
//
SRCEXP_EXPRBRA_DEFN(b, and_eq)
{
   return create_binary_assign
      (exprL, create_branch_and(exprL, exprR, context, pos), context, pos);
}

// EOF

