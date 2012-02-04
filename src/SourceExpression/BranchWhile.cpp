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
// SourceExpression handling of do-while and while loops.
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
// SourceExpression_BranchWhile
//
class SourceExpression_BranchWhile : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BranchWhile, SourceExpression);

public:
   SourceExpression_BranchWhile
   (bool postCond, SourceExpression * exprCond, SourceExpression *exprBody,
    SRCEXP_EXPR_ARGS);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer exprCond;
   SourceExpression::Pointer exprBody;

   bool postCond;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_do
//
SRCEXP_EXPRBRA_DEFN(2, do)
{
   return new SourceExpression_BranchWhile(true, exprCond, exprBody, context,
                                           position);
}

//
// SourceExpression::create_branch_while
//
SRCEXP_EXPRBRA_DEFN(2, while)
{
   return new SourceExpression_BranchWhile(false, exprCond, exprBody, context,
                                           position);
}

//
// SourceExpression_BranchWhile::SourceExpression_BranchWhile
//
SourceExpression_BranchWhile::
SourceExpression_BranchWhile
(bool _postCond, SourceExpression *_exprCond, SourceExpression *_exprBody,
 SRCEXP_EXPR_PARM)
 : Super(SRCEXP_EXPR_PASS),
   exprCond(_exprCond), exprBody(_exprBody), postCond(_postCond)
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
      VariableType const *type     = VariableType::get_vt_void();

      if (typeBody != type)
         exprBody = create_value_cast_implicit
                    (exprBody, type, context, position);
   }
}

void SourceExpression_BranchWhile::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   bigsint               sizeCond = exprCond->getType()->size(position);
   VariableData::Pointer destCond = VariableData::create_stack(sizeCond);

   std::string labelBody     = label + "_body";
   std::string labelContinue = context->getLabelContinue(position);

   if (!postCond)
      objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(labelContinue));

   objects->addLabel(labelBody);
   exprBody->makeObjects(objects, VariableData::create_void(0));

   objects->addLabel(labelContinue);
   exprCond->makeObjects(objects, destCond);
   objects->setPosition(position);
   objects->addToken(OCODE_BRANCH_TRUE, objects->getValue(labelBody));

   objects->addLabel(context->getLabelBreak(position));
}

// EOF

