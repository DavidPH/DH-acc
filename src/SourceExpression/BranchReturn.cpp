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
   return new SourceExpression_BranchReturn(expr, context, pos);
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
      expr = create_value_cast_implicit(expr, type, context, pos);
}

//
// SourceExpression_BranchReturn::virtual_makeObjects
//
void SourceExpression_BranchReturn::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   // How many bytes are we returning?
   bigsint retnSize = expr->getType()->getSize(pos);

   // Evaluate the returned expression.
   VariableData::Pointer tmp;
   if(retnSize == 0)
   {
      tmp = VariableData::create_void(0);
      expr->makeObjects(objects, tmp);
   }
   else if(retnSize == 1 && target_type == TARGET_ZDoom)
   {
      tmp = VariableData::create_stack(1);
      expr->makeObjects(objects, tmp);
   }
   else
   {
      ObjectExpression::Pointer objArray = objects->getValue(option_auto_array);

      // Use the end of the auto-array for extended return data.
      tmp = VariableData::create_array(retnSize, VariableData::SA_WORLD,
         objArray, create_value_int(-retnSize, context, pos));
      expr->makeObjects(objects, tmp);

      // But the first value needs to be on the stack.
      objects->setPosition(pos);
      objects->addToken(OCODE_GET_IMM, objects->getValue(-retnSize));
      objects->addToken(OCODE_GET_WLDARR, objArray);
   }

   objects->setPosition(pos);

   // Add the branch instruction.
   switch (ct)
   {
   case SourceContext::CT_FUNCTION:
      if(retnSize == 0)
         objects->addToken(OCODE_JMP_RET_NIL);
      else
         objects->addToken(OCODE_JMP_RET);

      break;

   case SourceContext::CT_NAMESPACE:
      objects->addToken(OCODE_ACS_SCRIPT_TERMINATE);
      break;

   case SourceContext::CT_SCRIPT:
      make_objects_auto_free(objects, context);

      if (retnSize != 0)
         objects->addToken(OCODE_ACSE_SCRIPT_SETRETURN);

      objects->addToken(OCODE_ACS_SCRIPT_TERMINATE);
      break;

   default:
      ERROR_NP("invalid CT");
   }
}

// EOF

