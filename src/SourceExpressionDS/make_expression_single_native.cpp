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
// DS handling of native.
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
// SourceExpressionDS::make_expression_single_native
//
SRCEXPDS_EXPRSINGLE_DEFN(native)
{
   // nativeName
   std::string nativeName = in->get(SourceTokenC::TT_IDENTIFIER).getData();

   // nativeArgTypes nativeReturn
   VariableType::Vector nativeArgTypes;
   VariableType::Pointer nativeReturn;
   make_expression_arglist(in, blocks, context, &nativeArgTypes, &nativeReturn);

   // nativeNumber
   in->get(SourceTokenC::TT_OP_AT);
   bigsint nativeNumber = get_bigsint(in->get(SourceTokenC::TT_INTEGER));

   // nativeObject
   ObjectExpression::Pointer nativeObject =
      ObjectExpression::create_value_int(nativeNumber, token.getPosition());

   // nativeVarType
   VariableType::Reference nativeVarType =
      VariableType::get_bt_native(nativeArgTypes, nativeReturn);

   // nativeVariable
   SourceVariable::Pointer nativeVariable =
      SourceVariable::create_constant
      (nativeName, nativeVarType, nativeObject, token.getPosition());

   context->addVariable(nativeVariable);
   return create_value_variable(nativeVariable, context, token.getPosition());
}

// EOF

