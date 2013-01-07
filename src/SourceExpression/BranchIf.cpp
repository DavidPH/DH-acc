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
// SourceExpression handling of for if/else blocks.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

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
// SourceExpression_BranchIf
//
class SourceExpression_BranchIf : public SourceExpression
{
   CounterPreambleNoClone(SourceExpression_BranchIf, SourceExpression);

public:
   SourceExpression_BranchIf
   (SourceExpression *exprCond, SourceExpression *exprBody,
    SRCEXP_EXPR_ARGS);

   SourceExpression_BranchIf
   (SourceExpression *exprCond, SourceExpression *exprBody,
    SourceExpression *exprElse, SRCEXP_EXPR_ARGS);

   virtual VariableType::Reference getType() const;

   //
   // isSideEffect
   //
   virtual bool isSideEffect() const
   {
      // TODO: Surely there are cases where there aren't.
      return true;
   }

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer exprCond;
   SourceExpression::Pointer exprBody;
   SourceExpression::Pointer exprElse;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_if
//
SRCEXP_EXPRBRA_DEFN(2, if)
{
   return new SourceExpression_BranchIf
      (exprCond, exprBody, NULL, context, pos);
}

//
// SourceExpression::create_branch_if
//
SRCEXP_EXPRBRA_DEFN(3, if)
{
   return new SourceExpression_BranchIf
      (exprCond, exprBody, exprElse, context, pos);
}

//
// SourceExpression_BranchIf::SourceExpression_BranchIf
//
SourceExpression_BranchIf::
SourceExpression_BranchIf
(SourceExpression *_exprCond, SourceExpression *_exprBody,
 SourceExpression *_exprElse, SRCEXP_EXPR_PARM)
 : Super(SRCEXP_EXPR_PASS),
   exprCond(_exprCond), exprBody(_exprBody), exprElse(_exprElse)
{
   {
      VariableType::Reference type = VariableType::get_bt_bit_sft();

      exprCond = create_value_cast_implicit(exprCond, type, context, pos);
   }

   if (exprElse)
   {
      VariableType::Reference type =
         get_promoted_type(exprBody->getType(), exprElse->getType(), pos);

      exprBody = create_value_cast_implicit(exprBody, type, context, pos);
      exprElse = create_value_cast_implicit(exprElse, type, context, pos);
   }
   else
   {
      VariableType::Reference type = VariableType::get_bt_void();

      exprBody = create_value_cast_implicit(exprBody, type, context, pos);
   }
}

//
// SourceExpression_BranchIf::getType
//
VariableType::Reference SourceExpression_BranchIf::getType() const
{
   return exprBody->getType();
}

//
// SourceExpression_BranchIf::virtual_makeObjects
//
void SourceExpression_BranchIf::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   bigsint               sizeCond = exprCond->getType()->getSize(pos);
   VariableData::Pointer destCond = VariableData::create_stack(sizeCond);

   ObjectExpression::Pointer objCond;

   // If the condition is known at compile-time, do the branch now.
   if(exprCond->canMakeObject() && (objCond = exprCond->makeObject())->canResolve())
   {
      if(objCond->resolveINT())
         exprBody->makeObjects(objects, dst);
      else if(exprElse)
         exprElse->makeObjects(objects, dst);

      return;
   }

   std::string label = context->makeLabel();
   std::string labelBody = label + "_body";
   std::string labelElse = label + "_else";
   std::string labelDone = label + "_done";

   exprCond->makeObjects(objects, destCond);
   objects->setPosition(pos);
   objects->addToken(OCODE_JMP_NIL,
                     objects->getValue(exprElse ? labelElse : labelDone));

   objects->addLabel(labelBody);
   exprBody->makeObjects(objects, dst);

   if (exprElse)
   {
      objects->setPosition(pos);
      objects->addToken(OCODE_JMP_IMM, objects->getValue(labelDone));
      objects->addLabel(labelElse);
      exprElse->makeObjects(objects, dst);
   }

   objects->addLabel(labelDone);
}

// EOF

