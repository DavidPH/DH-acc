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
// SourceExpression handling of "operator return".
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../ost_type.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BranchReturn
//
class SourceExpression_BranchReturn : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BranchReturn,
                                   SourceExpression);

public:
   SourceExpression_BranchReturn(SRCEXP_EXPRUNA_ARGS);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer expr;
   SourceContext::ContextType ct;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_return
//
SRCEXP_EXPRBRA_DEFN(u, return)
{
   return new SourceExpression_BranchReturn(expr, context, position);
}

//
// SourceExpression_BranchReturn::SourceExpression_BranchReturn
//
SourceExpression_BranchReturn::
SourceExpression_BranchReturn(SRCEXP_EXPRUNA_PARM)
                              : Super(SRCEXP_EXPR_PASS),
                                expr(_expr), ct(context->getTypeRoot())
{
   VariableType::Reference type = context->getReturnType();

   if (expr->getType() != type)
      expr = create_value_cast_implicit(expr, type, context, position);
}

//
// SourceExpression_BranchReturn::virtual_makeObjects
//
void SourceExpression_BranchReturn::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   bigsint srcSize = expr->getType()->getSize(position);
   if (srcSize && target_type == TARGET_ZDoom)
      --srcSize;
   VariableData::Pointer src = VariableData::create_stack(srcSize);

   expr->makeObjects(objects, src);

   objects->setPosition(position);

   bigsint retnSize(expr->getType()->getSize(position));

   if (target_type != TARGET_ZDoom)
   {
      // FIXME: Should be based on type.
      for (bigsint i(1); i <= retnSize; ++i)
         objects->addToken(OCODE_SET_AUTO32I, objects->getValue(-i));
   }
   else
   {
      // FIXME: Should be based on type.
      for (bigsint i(1); i < retnSize; ++i)
         objects->addToken(OCODE_SET_AUTO32I, objects->getValue(-i));
   }

   switch (ct)
   {
   case SourceContext::CT_BLOCK:
      objects->addToken(OCODE_ACS_SCRIPT_TERMINATE);
      break;

   case SourceContext::CT_FUNCTION:
      if (target_type != TARGET_ZDoom)
         objects->addToken(OCODE_BRANCH_GOTO);
      else if (retnSize == 0)
         objects->addToken(OCODE_ACSE_FUNC_RETNVOID);
      else
         objects->addToken(OCODE_ACSE_FUNC_RETN);

      break;

   case SourceContext::CT_SCRIPT:
      if (retnSize != 0)
         objects->addToken(OCODE_ACSE_SCRIPT_SETRETURN);

      objects->addToken(OCODE_ACS_SCRIPT_TERMINATE);
      break;

   default:
      throw SourceException("invalid CT", position, getName());
   }
}

// EOF

