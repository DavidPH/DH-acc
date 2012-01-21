//-----------------------------------------------------------------------------
//
// Copyright(C) 2011 David Hill
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
   return new SourceExpression_BranchSwitch(exprCond, exprBody, context,
                                            position);
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
      VariableType const *typeCond = exprCond->getType();
      VariableType const *type     = VariableType::get_vt_int();

      if (typeCond != type)
         exprCond = create_value_cast(exprCond, type, context, position);
   }

   {
      VariableType const *typeBody = exprBody->getType();
      VariableType const *type     = VariableType::get_vt_void();

      if (typeBody != type)
         exprBody = create_value_cast(exprBody, type, context, position);
   }
}

//
// SourceExpression_BranchSwitch::virtual_makeObjects
//
void SourceExpression_BranchSwitch::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   // Build case data.
   std::vector<bigsint> cases = context->getCases(position);
   std::string caseDefault = context->getLabelCaseDefault(position);

   std::vector<ObjectExpression::Pointer> args;
   args.reserve(cases.size() * 2);

   for (size_t i(0); i < cases.size(); ++i)
   {
      bigsint     caseNum = cases[i];
      std::string caseLab = context->getLabelCase(caseNum, position);

      args.push_back(objects->getValue(caseNum));
      args.push_back(objects->getValue(caseLab));
   }

   // Generate condition.
   bigsint               sizeCond = exprCond->getType()->size(position);
   VariableData::Pointer destCond = VariableData::create_stack(sizeCond);

   exprCond->makeObjects(objects, destCond);

   objects->setPosition(position);

   // Generate jump table.
   objects->addToken(OCODE_BRANCH_TABLE, args);

   objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(caseDefault));

   // Generate cases.
   exprBody->makeObjects(objects, VariableData::create_void(0));

   // Add default target if none specified.
   if (!context->hasLabelCaseDefault())
      objects->addLabel(caseDefault);

   // Add break target.
   objects->addLabel(context->getLabelBreak(position));
}

// EOF

