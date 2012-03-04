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
// DS handling of linespec.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_linespec
//
SRCEXPDS_EXPRSINGLE_DEFN(linespec)
{
   // linespecName
   std::string linespecName = in->get(SourceTokenC::TT_IDENTIFIER).data;

   // linespecArgTypes linespecReturn
   VariableType::Vector linespecArgTypes;
   VariableType::Pointer linespecReturn;
   make_expression_arglist
   (in, blocks, context, &linespecArgTypes, &linespecReturn);

   // linespecNumber
   in->get(SourceTokenC::TT_OP_AT);
   bigsint linespecNumber =
      make_expression_single(in, blocks, context)->makeObject()->resolveInt();

   // linespecObject
   ObjectExpression::Pointer linespecObject =
      ObjectExpression::create_value_int(linespecNumber, token.pos);

   // linespecVarType
   VariableType::Reference linespecVarType =
      VariableType::get_bt_linespec(linespecArgTypes, linespecReturn);

   // linespecVariable
   SourceVariable::Pointer linespecVariable =
      SourceVariable::create_constant
      (linespecName, linespecVarType, linespecObject, token.pos);

   context->addFunction(linespecVariable);
   return create_value_variable(linespecVariable, context, token.pos);
}

// EOF

