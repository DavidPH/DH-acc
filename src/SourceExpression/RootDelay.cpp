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
// SourceExpression handling of execution delaying.
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
// SourceExpression_RootDelay
//
class SourceExpression_RootDelay : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_RootDelay,
                                   SourceExpression);

public:
   SourceExpression_RootDelay(SRCEXP_EXPRUNA_ARGS);

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer expr;
   bigsint stack;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_root_delay
//
SourceExpression::Pointer SourceExpression::
create_root_delay(SRCEXP_EXPRUNA_ARGS)
{
   return new SourceExpression_RootDelay(expr, context, position);
}

//
// SourceExpression_RootDelay::SourceExpression_RootDelay
//
SourceExpression_RootDelay::
SourceExpression_RootDelay(SRCEXP_EXPRUNA_PARM)
                           : Super(SRCEXP_EXPR_PASS),
                             expr(_expr),
                             stack(context->getLimit(SourceVariable::SC_AUTO))
{
   VariableType const *type = VariableType::get_vt_int();

   if (expr->getType() != type)
      expr = create_value_cast(expr, type, context, position);
}

//
// SourceExpression_RootDelay::virtual_makeObjects
//
void SourceExpression_RootDelay::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   expr->makeObjects
         (objects, VariableData::create_stack(expr->getType()->size(position)));

   objects->setPosition(position);

   ObjectExpression::Pointer stackObj(objects->getValue(stack));
   objects->addToken(OCODE_ADDR_STACK_ADD_IMM, stackObj);
   objects->addToken(OCODE_ACS_WAIT_TICS);
   objects->addToken(OCODE_ADDR_STACK_SUB_IMM, stackObj);
}

// EOF

