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
// SourceExpression handling of "operator ()".
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
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
   SourceExpression_BranchCall(SourceExpression *expr,
                               SourceExpression::Vector const &args,
                               SRCEXP_EXPR_ARGS);

   virtual VariableType const *getType() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Vector args;
   SourceExpression::Pointer expr;
   bigsint stack;
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
SourceExpression_BranchCall::
SourceExpression_BranchCall
(SourceExpression *_expr, SourceExpression::Vector const &_args,
 SRCEXP_EXPR_PARM)
 : Super(SRCEXP_EXPR_PASS),
   args(_args), expr(_expr), stack(context->getLimit(SourceVariable::SC_AUTO))
{
   VariableType const *type(expr->getType());

   if (args.size() != type->types.size())
      throw SourceException("incorrect arg count", position, getName());

   for (size_t i(0); i < args.size(); ++i)
   {
      if (args[i]->getType() != type->types[i])
         args[i] = create_value_cast(args[i], type->types[i], context, position);
   }
}

//
// SourceExpression_BranchCall::getType
//
VariableType const * SourceExpression_BranchCall::getType() const
{
   return expr->getType()->callType;
}

//
// SourceExpression_BranchCall::virtual_makeObjects
//
void SourceExpression_BranchCall::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   std::string labelReturn = label + "_retn";

   make_objects_call(objects, dst, expr, args, objects->getValue(stack),
                     labelReturn, position);
}

// EOF

