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
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BranchFor,
                                   SourceExpression);

public:
   SourceExpression_BranchFor
   (SourceExpression *exprCond, SourceExpression *exprBody,
    SourceExpression *exprIter, SourceExpression *exprInit,
    SRCEXP_EXPR_ARGS);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer exprCond;
   SourceExpression::Pointer exprBody;
   SourceExpression::Pointer exprIter;
   SourceExpression::Pointer exprInit;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_for
//
SRCEXP_EXPRBRA_DEFN(4, for)
{
   return new SourceExpression_BranchFor
              (exprCond, exprBody, exprIter, exprInit, context, position);
}

//
// SourceExpression_BranchFor::SourceExpression_BranchFor
//
SourceExpression_BranchFor::
SourceExpression_BranchFor
(SourceExpression *_exprCond, SourceExpression *_exprBody,
 SourceExpression *_exprIter, SourceExpression *_exprInit,
 SRCEXP_EXPR_PARM)
 : Super(SRCEXP_EXPR_PASS),
   exprCond(_exprCond), exprBody(_exprBody),
   exprIter(_exprIter), exprInit(_exprInit)
{
   {
      VariableType const *typeCond = exprCond->getType();
      VariableType const *type     = VariableType::get_vt_boolsoft();

      if (typeCond != type)
         exprCond = create_value_cast_implicit
                    (exprCond, type, context, position);
   }

   {
      VariableType const *typeBody = exprBody->getType();
      VariableType const *typeIter = exprIter->getType();
      VariableType const *typeInit = exprInit->getType();
      VariableType const *type     = VariableType::get_vt_void();

      if (typeBody != type)
         exprBody = create_value_cast_implicit
                    (exprBody, type, context, position);

      if (typeIter != type)
         exprIter = create_value_cast_implicit
                    (exprIter, type, context, position);

      if (typeInit != type)
         exprInit = create_value_cast_implicit
                    (exprInit, type, context, position);
   }
}


//
// SourceExpression_BranchFor::virtual_makeObjects
//
void SourceExpression_BranchFor::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   bigsint               sizeCond = exprCond->getType()->size(position);
   VariableData::Pointer destCond = VariableData::create_stack(sizeCond);
   VariableData::Pointer sink     = VariableData::create_void(0);

   std::string labelCond = label + "_cond";
   std::string labelBody = label + "_body";

   exprInit->makeObjects(objects, sink);
   objects->setPosition(position);
   objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(labelCond));

   objects->addLabel(labelBody);
   exprBody->makeObjects(objects, sink);

   objects->addLabel(context->getLabelContinue(position));
   exprIter->makeObjects(objects, sink);

   objects->addLabel(labelCond);
   exprCond->makeObjects(objects, destCond);
   objects->setPosition(position);
   objects->addToken(OCODE_BRANCH_TRUE, objects->getValue(labelBody));

   objects->addLabel(context->getLabelBreak(position));
}

// EOF

