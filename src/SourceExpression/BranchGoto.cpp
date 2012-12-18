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
// SourceExpression handling of "operator goto" and "operator goto_dyn".
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
// SourceExpression_BranchGoto
//
class SourceExpression_BranchGoto : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BranchGoto,
                                   SourceExpression);

public:
   SourceExpression_BranchGoto(std::string const &label, SRCEXP_EXPR_ARGS);
   SourceExpression_BranchGoto(SRCEXP_EXPRUNA_ARGS);

   //
   // isSideEffect
   //
   // Branching isn't strictly a "side-effect", but it needs to happen.
   //
   virtual bool isSideEffect() const
   {
      return true;
   }

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer expr;
   std::string labelGoto;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_break
//
SRCEXP_EXPRBRA_DEFN(0, break)
{
   return new SourceExpression_BranchGoto
      (context->getLabelBreak(pos), context, pos);
}

//
// SourceExpression::create_branch_break
//
SRCEXP_EXPRBRA_DEFN(s, break)
{
   return new SourceExpression_BranchGoto(context->getLabelBreak(value, pos),
                                          context, pos);
}

//
// SourceExpression::create_branch_continue
//
SRCEXP_EXPRBRA_DEFN(0, continue)
{
   return new SourceExpression_BranchGoto
      (context->getLabelContinue(pos), context, pos);
}

//
// SourceExpression::create_branch_continue
//
SRCEXP_EXPRBRA_DEFN(s, continue)
{
   return new SourceExpression_BranchGoto(context->getLabelContinue(value, pos),
                                          context, pos);
}

//
// SourceExpression::create_branch_goto
//
SRCEXP_EXPRBRA_DEFN(s, goto)
{
   return new SourceExpression_BranchGoto(value, context, pos);
}


//
// SourceExpression::create_branch_goto
//
SRCEXP_EXPRBRA_DEFN(u, goto)
{
   return new SourceExpression_BranchGoto(expr, context, pos);
}

//
// SourceExpression_BranchGoto::SourceExpression_BranchGoto
//
SourceExpression_BranchGoto::
SourceExpression_BranchGoto(std::string const &_label, SRCEXP_EXPR_PARM)
                            : Super(SRCEXP_EXPR_PASS),
                              labelGoto(_label)
{
}

//
// SourceExpression_BranchGoto::SourceExpression_BranchGoto
//
SourceExpression_BranchGoto::
SourceExpression_BranchGoto(SRCEXP_EXPRUNA_PARM)
                            : Super(SRCEXP_EXPR_PASS), expr(_expr)
{
   expr = create_value_cast_implicit
          (expr, VariableType::get_bt_label(), context, pos);
}

//
// SourceExpression_BranchGoto::virtual_makeObjects
//
void SourceExpression_BranchGoto::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   if (expr)
   {
      VariableData::Pointer src = VariableData::create_stack(expr->getType()->getSize(pos));

      expr->makeObjects(objects, src);
      objects->setPosition(pos);
      objects->addToken(OCODE_JMP);
   }
   else
      objects->addToken(OCODE_JMP_IMM, objects->getValue(labelGoto));
}

// EOF

