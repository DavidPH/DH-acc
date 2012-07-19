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
// SourceExpression handling of switch blocks.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../ost_type.hpp"
#include "../SourceContext.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BranchSwitch
//
class SourceExpression_BranchSwitch : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BranchSwitch,
                                   SourceExpression);

public:
   SourceExpression_BranchSwitch
   (SourceExpression *exprCond, SourceExpression *exprBody,
    SRCEXP_EXPR_ARGS);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer exprCond;
   SourceExpression::Pointer exprBody;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_switch
//
SRCEXP_EXPRBRA_DEFN(2, switch)
{
   return new SourceExpression_BranchSwitch(exprCond, exprBody, context, pos);
}

//
// SourceExpression_BranchSwitch::SourceExpression_BranchSwitch
//
SourceExpression_BranchSwitch::
SourceExpression_BranchSwitch
(SourceExpression *_exprCond, SourceExpression *_exprBody,
 SRCEXP_EXPR_PARM)
 : Super(SRCEXP_EXPR_PASS),
   exprCond(_exprCond), exprBody(_exprBody)
{
   {
      VariableType::Reference type = VariableType::get_bt_int();

      exprCond = create_value_cast_implicit(exprCond, type, context, pos);
   }

   {
      VariableType::Reference type = VariableType::get_bt_void();

      exprBody = create_value_cast_implicit(exprBody, type, context, pos);
   }
}

//
// SourceExpression_BranchSwitch::virtual_makeObjects
//
void SourceExpression_BranchSwitch::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   // Generate condition.
   bigsint               sizeCond = exprCond->getType()->getSize(pos);
   VariableData::Pointer destCond = VariableData::create_stack(sizeCond);

   exprCond->makeObjects(objects, destCond);

   objects->setPosition(pos);

   // Acquire case data.
   std::vector<bigsint> cases = context->getCases(pos);
   std::string caseDefault = context->getLabelCaseDefault(pos);

   std::vector<bigsint>::iterator caseItr, caseEnd = cases.end();

   // Generate jump table.
   if(cases.size() < 4 || target_type == TARGET_Hexen)
   {
      // Linear search via JMP_VAL.

      // Fall back to this for very small tables, since it is probably faster.
      // Or for Hexen which only has this method.

      for(caseItr = cases.begin(); caseItr != caseEnd; ++caseItr)
         objects->addToken(OCODE_JMP_VAL, objects->getValue(*caseItr),
                           objects->getValue(context->getLabelCase(*caseItr, pos)));
   }
   else
   {
      // Binary search via JMP_TAB.

      ObjectExpression::Vector args;
      args.reserve(cases.size() * 2);

      for(caseItr = cases.begin(); caseItr != caseEnd; ++caseItr)
      {
         args.push_back(objects->getValue(*caseItr));
         args.push_back(objects->getValue(context->getLabelCase(*caseItr, pos)));
      }

      // Generate jump table.
      objects->addToken(OCODE_JMP_TAB, args);
   }

   objects->addToken(OCODE_STK_DROP);
   objects->addToken(OCODE_JMP_IMM, objects->getValue(caseDefault));

   // Generate cases.
   exprBody->makeObjects(objects, VariableData::create_void(0));

   // Add default target if none specified.
   if(!context->hasLabelCaseDefault())
      objects->addLabel(caseDefault);

   // Add break target.
   objects->addLabel(context->getLabelBreak(pos));
}

// EOF

