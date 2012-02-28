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
// DS handling of function.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../ObjectExpression.hpp"
#include "../option.hpp"
#include "../ost_type.hpp"
#include "../SourceContext.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

extern bool option_string_func;
static option::option_dptr<bool> option_string_func_handle
('\0', "string-func", "features",
 "Enables __func__ constants for functions and scripts. On by default.", NULL,
 &option_string_func);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern bool option_function_autoargs;
bool option_string_func = true;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_extern_function
//
SRCEXPDS_EXPRSINGLE_DEFN(extern_function)
{
   // functionArgClass
   SourceVariable::StorageClass functionArgClass;

   if (option_function_autoargs || target_type != TARGET_ZDoom)
      functionArgClass = SourceVariable::SC_AUTO;
   else
      functionArgClass = SourceVariable::SC_REGISTER;

   // functionName
   std::string functionName = in->get(SourceTokenC::TT_IDENTIFIER).data;

   // functionLabel
   std::string functionLabel = "function_" + functionName;

   // functionNameObject
   std::string functionNameObject= functionLabel + "_id";

   // functionArgTypes/Count functionReturn
   VariableType::Vector functionArgTypes;
   int functionArgCount;
   VariableType::Pointer functionReturn;
   make_expression_arglist
   (in, blocks, context, &functionArgTypes, NULL, &functionArgCount, NULL,
    &functionReturn, functionArgClass);

   // Don't count automatic variable args.
   if (option_function_autoargs) functionArgCount = 0;

   // functionVarType
   VariableType::Reference functionVarType =
      VariableType::get_bt_function(functionArgTypes, functionReturn);

   // functionVariable
   SourceVariable::Pointer functionVariable;
   if (target_type != TARGET_ZDoom)
   {
      functionVariable =
         SourceVariable::create_constant
         (functionName, functionVarType, functionLabel, token.pos);
   }
   else
   {
      functionVariable =
         SourceVariable::create_constant
         (functionName, functionVarType, functionNameObject, token.pos);
   }
   context->addVariable(functionVariable);

   ObjectExpression::add_function
   (functionNameObject, functionLabel, functionArgCount, functionArgCount,
    functionReturn->getSize(token.pos), true);

   return create_value_variable(functionVariable, context, token.pos);
}

//
// SourceExpressionDS::make_expression_single_function
//
SRCEXPDS_EXPRSINGLE_DEFN(function)
{
   // functionArgClass
   SourceVariable::StorageClass functionArgClass;

   if (option_function_autoargs || target_type != TARGET_ZDoom)
      functionArgClass = SourceVariable::SC_AUTO;
   else
      functionArgClass = SourceVariable::SC_REGISTER;

   // functionContext
   SourceContext::Reference functionContext =
      SourceContext::create(context, SourceContext::CT_FUNCTION);

   // functionName
   SourceTokenC functionNameToken = in->get(SourceTokenC::TT_IDENTIFIER);
   std::string functionName = functionNameToken.data;

   // __func__
   if (option_string_func)
   {
      std::string funcVarData = ObjectData_String::add(functionName);

      SourceVariable::Pointer funcVar =
         SourceVariable::create_constant
         ("__func__", VariableType::get_bt_string(), funcVarData,
		functionNameToken.pos);

      functionContext->addVariable(funcVar);
   }

   // functionLabel
   std::string functionLabel;
   if (token.data != "__extfunc")
      functionLabel += context->makeLabel();
   functionLabel += "function_";
   functionLabel += functionName;

   // functionNameObject
   std::string functionNameObject = functionLabel + "_id";

   // functionArgTypes/Names/Count functionReturn
   VariableType::Vector functionArgTypes;
   VariableType::VecStr functionArgNames;
   int functionArgCount;
   VariableType::Pointer functionReturn;
   make_expression_arglist
   (in, blocks, context, &functionArgTypes, &functionArgNames,
    &functionArgCount, functionContext, &functionReturn, functionArgClass);

   // Don't count automatic variable args.
   if (option_function_autoargs) functionArgCount = 0;

   // functionVarType
   VariableType::Reference functionVarType =
      VariableType::get_bt_function(functionArgTypes, functionReturn);

   // functionVariable
   // Before functionExpression to enable recursion.
   SourceVariable::Pointer functionVariable;
   if (target_type != TARGET_ZDoom)
   {
      functionVariable =
         SourceVariable::create_constant
         (functionName, functionVarType, functionLabel, token.pos);
   }
   else
   {
      functionVariable =
         SourceVariable::create_constant
         (functionName, functionVarType, functionNameObject, token.pos);
   }
   context->addVariable(functionVariable);

   // functionExpression
   SourceExpression::Pointer functionExpression =
      make_expression_single(in, blocks, functionContext);
   functionExpression->addLabel(functionLabel);
   blocks->push_back(functionExpression);

   SourceExpression::Pointer functionExprData =
      create_value_data_garbage(functionReturn, functionContext, token.pos);
   SourceExpression::Pointer functionExprRetn =
      create_branch_return(functionExprData, functionContext, token.pos);
   blocks->push_back(functionExprRetn);

   // functionVarCount
   int functionVarCount =
      functionContext->getLimit(SourceVariable::SC_REGISTER);

   ObjectExpression::add_function
   (functionNameObject, functionLabel, functionArgCount, functionVarCount,
    functionReturn->getSize(token.pos), false);

   return create_value_variable(functionVariable, context, token.pos);
}

// EOF

