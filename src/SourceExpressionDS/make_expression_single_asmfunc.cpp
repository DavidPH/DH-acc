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
// DS handling of asmfunc.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../ObjectCode.hpp"
#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_asmfunc
//
SRCEXPDS_EXPRSINGLE_DEFN(asmfunc)
{
   VariableType::Pointer asmfuncReturn;

   // asmfuncReturn
   if (!in->peekType(SourceTokenC::TT_IDENTIFIER) ||
       is_expression_type(in->peek().data, context))
      asmfuncReturn = make_expression_type(in, blocks, context);

   // asmfuncName
   std::string asmfuncName = in->get(SourceTokenC::TT_IDENTIFIER).data;

   // asmfuncArgTypes asmfuncReturn
   VariableType::Vector asmfuncArgTypes;
   make_expression_arglist(in, blocks, context, &asmfuncArgTypes,
                           asmfuncReturn ? NULL : &asmfuncReturn);

   // asmfuncOCode
   SourceTokenC asmfuncOCodeToken;
   ObjectCodeSet asmfuncOCode;

   in->get(SourceTokenC::TT_OP_AT);
   in->get(SourceTokenC::TT_IDENTIFIER, "__ocode");
   in->get(SourceTokenC::TT_OP_PARENTHESIS_O);

   asmfuncOCodeToken = in->get(SourceTokenC::TT_IDENTIFIER);
   asmfuncOCode.ocode =
      ocode_get_code(asmfuncOCodeToken.data, asmfuncOCodeToken.pos);

   if (!in->peekType(SourceTokenC::TT_OP_PARENTHESIS_C))
   {
      in->get(SourceTokenC::TT_OP_COMMA);

      asmfuncOCodeToken = in->get(SourceTokenC::TT_IDENTIFIER);
      asmfuncOCode.ocode_imm =
         ocode_get_code(asmfuncOCodeToken.data, asmfuncOCodeToken.pos);
   }

   in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

   // asmfuncObject
   ObjectExpression::Pointer asmfuncObject =
      ObjectExpression::create_value_ocode(asmfuncOCode, token.pos);

   // asmfuncVarType
   VariableType::Reference asmfuncVarType =
      VariableType::get_bt_asmfunc(asmfuncArgTypes, asmfuncReturn);

   // asmfuncVariable
   SourceVariable::Pointer asmfuncVariable =
      SourceVariable::create_constant
      (asmfuncName, asmfuncVarType, asmfuncObject, token.pos);

   context->addFunction(asmfuncVariable);
   return create_value_variable(asmfuncVariable, context, token.pos);
}

// EOF

