//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 David Hill
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
// SourceExpression handling of function-designators.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceFunction.hpp"
#include "../SourceVariable.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_ValueFunction
//
class SourceExpression_ValueFunction : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE
   (SourceExpression_ValueFunction, SourceExpression);

public:
   //
   // ::SourceExpression_ValueFunction
   //
   SourceExpression_ValueFunction(SourceFunction *_func, SRCEXP_EXPR_PARM)
    : Super(SRCEXP_EXPR_PASS), func(_func)
   {
   }

   //
   // ::SourceExpression_ValueFunction
   //
   SourceExpression_ValueFunction(std::string const &_name, SRCEXP_EXPR_PARM)
    : Super(SRCEXP_EXPR_PASS), name(_name)
   {
   }

   //
   // ::canGetData
   //
   virtual bool canGetData() const
   {
      return !!func;
   }

   //
   // ::canGetFunction
   //
   virtual bool canGetFunction() const
   {
      return !!func;
   }

   //
   // ::getData
   //
   virtual VariableData::Pointer getData() const
   {
      if(func)
         return func->var->getData();

      return Super::getData();
   }

   //
   // ::getFunction
   //
   virtual SourceFunction::Reference getFunction() const
   {
      if(func)
         return static_cast<SourceFunction::Reference>(func);

      return Super::getFunction();
   }

   //
   // ::getType
   //
   virtual VariableType::Reference getType() const
   {
      if(func)
         return func->var->getType();

      Error_NP("designator has no type");
   }

   //
   // ::makeExpressionFunction
   //
   virtual SourceExpression::Pointer makeExpressionFunction(
      VariableType::Vector const &types, ObjectExpression::Vector const &objs)
   {
      if(func)
         return this;

      return create_value_function(context->getFunction(name, pos, types, objs), context, pos);
   }

private:
   std::string name;
   SourceFunction::Pointer func;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_value_function
//
SRCEXP_EXPRVAL_DEFN(f, function)
{
   return new SourceExpression_ValueFunction(func, context, pos);
}

//
// SourceExpression::create_value_function
//
SRCEXP_EXPRVAL_DEFN(s, function)
{
   return new SourceExpression_ValueFunction(value, context, pos);
}

// EOF

