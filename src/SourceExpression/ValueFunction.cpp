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

#include "../SourceContext.hpp"
#include "../SourceException.hpp"


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
   SourceExpression_ValueFunction(std::string const &_name, SRCEXP_EXPR_PARM)
    : Super(SRCEXP_EXPR_PASS), name(_name)
   {
   }

   //
   // ::getType
   //
   virtual CounterReference<VariableType> getType() const
   {
      ERROR_NP("designator has no type");
   }

   //
   // ::makeExpressionFunction
   //
   virtual SourceExpression::Pointer makeExpressionFunction
   (std::vector<CounterPointer<VariableType> > const &types)
   {
      SourceVariable::Pointer func = context->getFunction(name, pos, types);
      return create_value_variable(func, context, pos);
   }

private:
   std::string name;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_value_function
//
SRCEXP_EXPRVAL_DEFN(s, function)
{
   return new SourceExpression_ValueFunction(value, context, pos);
}

// EOF

