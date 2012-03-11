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

#include "../ObjectData.hpp"
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

extern bool option_function_mangle_types;
static option::option_dptr<bool> option_function_mangle_types_handle
('\0', "function-mangle-types", "features",
 "Enables function name mangling based on types. On by default.",
 "Enables function name mangling based on types. Without this, overloading "
 "functions may not work. However, this mangling may be incompatible with "
 "other compilers. On by default.",
 &option_function_mangle_types);

extern bool option_string_func;
static option::option_dptr<bool> option_string_func_handle
('\0', "string-func", "features",
 "Enables __func__ constants for functions and scripts. On by default.", NULL,
 &option_string_func);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern bool option_function_autoargs;
bool option_function_mangle_types = true;
bool option_string_func = true;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

static void mangle_types(VariableType::Vector const &types, std::string &name)
{
   name += "$";
   VariableType::Vector::const_iterator type;
   for (type = types.begin(); type != types.end(); ++type)
      {name += '$'; (*type)->getNameMangled(name);}
}


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

   // functionArgTypes/Count functionReturn
   VariableType::Vector functionArgTypes;
   int functionArgCount;
   VariableType::Pointer functionReturn;
   make_expression_arglist
   (in, blocks, context, &functionArgTypes, NULL, &functionArgCount, NULL,
    &functionReturn, functionArgClass);
   // Don't count automatic variable args.
   if (option_function_autoargs) functionArgCount = 0;

   // functionLabel
   std::string functionLabel = "::" + functionName;
   if (option_function_mangle_types)
      mangle_types(functionArgTypes, functionLabel);

   // functionNameObject
   std::string functionNameObject = functionLabel + "_id";

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

   bool added = ObjectData_Function::add
   (functionNameObject, functionLabel, functionArgCount,
    functionReturn->getSize(token.pos), NULL);

   if (added)
      context->addFunction(functionVariable);

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

   // functionLabel
   std::string functionLabel = "::";
   if (token.data != "__extfunc")
      functionLabel += context->makeLabel();
   functionLabel += functionName;
   if (option_function_mangle_types)
      mangle_types(functionArgTypes, functionLabel);

   // functionNameObject
   std::string functionNameObject = functionLabel + "_id";

   // __func__
   if (option_string_func)
   {
      std::string funcVarData = functionName;

      if (option_function_mangle_types)
      {
         if (!functionArgTypes.empty())
         {
            funcVarData += '(';

            VariableType::Vector::iterator it = functionArgTypes.begin();
            funcVarData += make_string(*it);
            for (++it; it != functionArgTypes.end(); ++it)
            {
               funcVarData += ", ";
               funcVarData += make_string(*it);
            }

            funcVarData += ')';
         }
         else
            funcVarData += "(void)";

         funcVarData += " -> ";
         funcVarData += make_string(functionReturn);
      }

      funcVarData = ObjectData_String::add(funcVarData);

      SourceVariable::Pointer funcVar =
         SourceVariable::create_constant
         ("__func__", VariableType::get_bt_string(), funcVarData,
          functionNameToken.pos);

      functionContext->addVariable(funcVar);
   }

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

   bool added = ObjectData_Function::add
   (functionNameObject, functionLabel, functionArgCount,
    functionReturn->getSize(token.pos), functionContext);

   if (added)
      context->addFunction(functionVariable);

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

   return create_value_variable(functionVariable, context, token.pos);
}

// EOF

