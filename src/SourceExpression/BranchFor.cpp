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
// SourceExpression handling of for loops.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BranchFor
//
class SourceExpression_BranchFor : public SourceExpression
{
   CounterPreambleNoClone(SourceExpression_BranchFor, SourceExpression);

public:
   //
   // ::SourceExpression_BranchFor
   //
   SourceExpression_BranchFor(SourceExpression *_exprCond,
      SourceExpression *_exprBody, SourceExpression *_exprIter,
      SourceExpression *_exprInit, bool _postCond, SRCEXP_EXPR_PARM)
    : Super(SRCEXP_EXPR_PASS), exprCond(_exprCond), exprBody(_exprBody),
      exprIter(_exprIter), exprInit(_exprInit), postCond(_postCond)
   {
      VariableType::Reference type = VariableType::get_bt_bit_sft();

      if(exprCond) exprCond = create_value_cast_implicit(exprCond, type, context, pos);

      type = VariableType::get_bt_void();

      if(exprBody) exprBody = create_value_cast_implicit(exprBody, type, context, pos);

      if(exprIter) exprIter = create_value_cast_implicit(exprIter, type, context, pos);
      if(exprInit) exprInit = create_value_cast_implicit(exprInit, type, context, pos);
   }

   //
   // isSideEffect
   //
   virtual bool isSideEffect() const
   {
      // TODO: Surely there are cases where there aren't.
      return true;
   }

private:
   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      bigsint               sizeCond = exprCond->getType()->getSize(pos);
      VariableData::Pointer destCond = VariableData::create_stack(sizeCond);
      VariableData::Pointer sink     = VariableData::create_void(0);

      ObjectExpression::Pointer objCond;
      bigsint                   retCond;

      // Check for condition constant.
      if(exprCond->canMakeObject())
      {
         // And make sure it can actually be resolved...
         if((objCond = exprCond->makeObject())->canResolve())
            retCond = objCond->resolveINT();
         else
            objCond = NULL;
      }

      // Jump targets.
      std::string label = context->makeLabel();
      std::string labelCond = label + "_cond";
      std::string labelBody = label + "_body";

      // Loop initializer, if any.
      if(exprInit) exprInit->makeObjects(objects, sink);

      // For pre-loop condition, need to jump to the condition.
      objects->setPosition(pos);
      if(!postCond)
      {
         // But if the condition is known to be true, just keep going.
         if(!objCond || !retCond)
            objects->addToken(OCODE_JMP_IMM, objects->getValue(labelCond));
      }

      // Loop body.
      objects->addLabel(labelBody);
      exprBody->makeObjects(objects, sink);

      // Loop iterator, if any.
      objects->addLabel(context->getLabelContinue(pos));
      if(exprIter) exprIter->makeObjects(objects, sink);

      // Loop condition.
      objects->addLabel(labelCond);
      if(objCond)
      {
         if(retCond)
            objects->addToken(OCODE_JMP_IMM, objects->getValue(labelBody));
      }
      else
      {
         exprCond->makeObjects(objects, destCond);
         objects->setPosition(pos);
         objects->addToken(OCODE_JMP_TRU, objects->getValue(labelBody));
      }

      objects->addLabel(context->getLabelBreak(pos));
   }

   SourceExpression::Pointer exprCond, exprBody, exprIter, exprInit;

   bool postCond : 1;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_do
//
SRCEXP_EXPRBRA_DEFN(2, do)
{
   return new SourceExpression_BranchFor(exprCond, exprBody, NULL, NULL, true,
                                         context, pos);
}

//
// SourceExpression::create_branch_for
//
SRCEXP_EXPRBRA_DEFN(4, for)
{
   return new SourceExpression_BranchFor(exprCond, exprBody, exprIter,
                                         exprInit, false, context, pos);
}

//
// SourceExpression::create_branch_while
//
SRCEXP_EXPRBRA_DEFN(2, while)
{
   return new SourceExpression_BranchFor(exprCond, exprBody, NULL, NULL, false,
                                         context, pos);
}

// EOF

