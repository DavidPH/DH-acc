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
// SourceExpression handling of "operator ()".
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BranchCall
//
class SourceExpression_BranchCall : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BranchCall,
                                   SourceExpression);

public:
   SourceExpression_BranchCall
   (SourceExpression *expr, Vector const &args, SRCEXP_EXPR_ARGS);

   virtual VariableType::Reference getType() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Vector args;
   SourceExpression::Pointer expr;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_branch_call
//
SRCEXP_EXPRBRA_DEFN(a, call)
{
   return new SourceExpression_BranchCall(expr, args, context, position);
}

//
// SourceExpression_BranchCall::SourceExpression_BranchCall
//
SourceExpression_BranchCall::SourceExpression_BranchCall
(SourceExpression *_expr, Vector const &_args, SRCEXP_EXPR_PARM)
 : Super(SRCEXP_EXPR_PASS), args(_args), expr(_expr)
{
   VariableType::Reference type = expr->getType();
   VariableType::Vector const &types = type->getTypes();

   for (size_t i = 0; i < types.size(); ++i)
   {
      if (!types[i])
         ERROR_N(position, "variadic call");

      if (i >= args.size())
         ERROR_N(position, "incorrect arg count");

      args[i] = create_value_cast_implicit
                (args[i], types[i], context, position);
   }
}

//
// SourceExpression_BranchCall::getType
//
VariableType::Reference SourceExpression_BranchCall::getType() const
{
   return expr->getType()->getReturn();
}

//
// SourceExpression_BranchCall::virtual_makeObjects
//
void SourceExpression_BranchCall::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   make_objects_call(objects, dst, expr, args, context, position);
}

// EOF

