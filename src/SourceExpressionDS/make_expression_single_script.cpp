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

#include "../ObjectData.hpp"
#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableType.hpp"

#include <sstream>


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern bool option_script_autoargs;
extern bool option_string_func;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_extern_script
//
SRCEXPDS_EXPRSINGLE_DEFN(extern_script)
{
   // scriptArgClass
   SourceVariable::StorageClass scriptArgClass;

   if (option_script_autoargs)
      scriptArgClass = SourceVariable::SC_AUTO;
   else
      scriptArgClass = SourceVariable::SC_REGISTER;

   // scriptName
   std::string scriptNameSource = in->get(SourceTokenC::TT_IDENTIFIER).data;

   // scriptType
   ObjectData_Script::ScriptType scriptType;
   if (in->peekType(SourceTokenC::TT_IDENTIFIER))
   {
      SourceTokenC scriptTypeToken = in->get(SourceTokenC::TT_IDENTIFIER);
      scriptType =
         ObjectData_Script::get_type(scriptTypeToken.data, scriptTypeToken.pos);
   }
   else
   {
      scriptType = ObjectData_Script::ST_CLOSED;
   }

   // scriptFlags
   int scriptFlags = 0;
   while (true)
   {
      if (!in->peekType(SourceTokenC::TT_IDENTIFIER))
         break;

      SourceTokenC scriptFlagToken = in->get(SourceTokenC::TT_IDENTIFIER);
      scriptFlags |=
         ObjectData_Script::get_flag(scriptFlagToken.data, scriptFlagToken.pos);
   }

   // scriptArgTypes/Names/Count scriptReturn
   VariableType::Vector scriptArgTypes;
   int scriptArgCount;
   VariableType::Pointer scriptReturn;
   make_expression_arglist
   (in, blocks, context, &scriptArgTypes, NULL, &scriptArgCount, NULL,
    &scriptReturn, scriptArgClass);

   // scriptNumber
   bigsint scriptNumber;
   if (in->peekType(SourceTokenC::TT_OP_AT))
   {
      in->get(SourceTokenC::TT_OP_AT);
      scriptNumber =
         make_expression_single(in, blocks, context)->makeObject()->resolveInt();
   }
   else
   {
      scriptNumber = -1;
   }

   // scriptLabel
   std::string scriptLabel = scriptNameSource;

   // scriptNameObject
   std::string scriptNameObject = scriptLabel + "_id";

   // scriptVarType
   VariableType::Reference scriptVarType =
      VariableType::get_bt_script(scriptArgTypes, scriptReturn);

   // scriptVariable
   SourceVariable::Pointer scriptVariable =
      SourceVariable::create_constant
      (scriptNameSource, scriptVarType, scriptNameObject, token.pos);

   if (scriptNumber < 0)
      ObjectData_Script::add
      (scriptNameObject, scriptLabel, scriptType, scriptFlags, scriptArgCount,
       NULL);
   else
      ObjectData_Script::add
      (scriptNameObject, scriptLabel, scriptType, scriptFlags, scriptArgCount,
       NULL, scriptNumber);

   context->addVariable(scriptVariable);
   return create_value_variable(scriptVariable, context, token.pos);
}

//
// SourceExpressionDS::make_expression_single_script
//
SRCEXPDS_EXPRSINGLE_DEFN(script)
{
   // scriptArgClass
   SourceVariable::StorageClass scriptArgClass;

   if (option_script_autoargs)
      scriptArgClass = SourceVariable::SC_AUTO;
   else
      scriptArgClass = SourceVariable::SC_REGISTER;

   // scriptContext
   SourceContext::Reference scriptContext =
      SourceContext::create(context, SourceContext::CT_SCRIPT);

   // scriptName
   SourceTokenC scriptNameToken = in->get(SourceTokenC::TT_IDENTIFIER);
   std::string scriptName = scriptNameToken.data;

   // __func__
   if (option_string_func)
   {
      std::string funcVarData = ObjectData_String::add(scriptName);

      SourceVariable::Pointer funcVar =
         SourceVariable::create_constant
         ("__func__", VariableType::get_bt_string(), funcVarData,
		scriptNameToken.pos);

      scriptContext->addVariable(funcVar);
   }

   // scriptType
   ObjectData_Script::ScriptType scriptType;
   if (in->peekType(SourceTokenC::TT_IDENTIFIER))
   {
      SourceTokenC scriptTypeToken = in->get(SourceTokenC::TT_IDENTIFIER);
      scriptType =
         ObjectData_Script::get_type(scriptTypeToken.data, scriptTypeToken.pos);
   }
   else
   {
      scriptType = ObjectData_Script::ST_CLOSED;
   }

   // scriptFlags
   int scriptFlags = 0;
   while (true)
   {
      if (!in->peekType(SourceTokenC::TT_IDENTIFIER))
         break;

      SourceTokenC scriptFlagToken = in->get(SourceTokenC::TT_IDENTIFIER);
      scriptFlags |=
         ObjectData_Script::get_flag(scriptFlagToken.data, scriptFlagToken.pos);
   }

   // scriptArgTypes/Names/Count scriptReturn
   VariableType::Vector scriptArgTypes;
   VariableType::VecStr scriptArgNames;
   int scriptArgCount;
   VariableType::Pointer scriptReturn;
   make_expression_arglist
   (in, blocks, context, &scriptArgTypes, &scriptArgNames, &scriptArgCount,
    scriptContext, &scriptReturn, scriptArgClass);

   // scriptNumber
   bigsint scriptNumber;
   if (in->peekType(SourceTokenC::TT_OP_AT))
   {
      in->get(SourceTokenC::TT_OP_AT);
      scriptNumber =
         make_expression_single(in, blocks, context)->makeObject()->resolveInt();
   }
   else
   {
      scriptNumber = -1;
   }

   // scriptNameSource
   std::string scriptNameSource;
   if (scriptName == "auto")
   {
      if (scriptNumber < 0)
         throw SourceException("name auto requires explicit number",
						 token.pos, __func__);

      std::ostringstream oss;
      oss << "script" << scriptNumber;
      scriptNameSource = oss.str();
   }
   else if (scriptName != "void")
   {
      scriptNameSource = scriptName;
   }

   // scriptLabel
   std::string scriptLabel;
   if (token.data != "__extscript")
      scriptLabel += context->makeLabel();
   scriptLabel += scriptNameSource;

   // scriptNameObject
   std::string scriptNameObject = scriptLabel + "_id";

   // scriptVarType
   VariableType::Reference scriptVarType =
      VariableType::get_bt_script(scriptArgTypes, scriptReturn);

   // scriptVariable
   // Before scriptExpression to enable recursion.
   SourceVariable::Pointer scriptVariable =
      SourceVariable::create_constant
      (scriptNameSource, scriptVarType, scriptNameObject, token.pos);
   context->addVariable(scriptVariable);

   // scriptExpression
   SourceExpression::Pointer scriptExpression =
      create_root_script(scriptVarType, scriptContext, token.pos);
   scriptExpression->addLabel(scriptLabel);
   blocks->push_back(scriptExpression);

   blocks->push_back(make_expression_single(in, blocks, scriptContext));

   SourceExpression::Pointer scriptExprData =
      create_value_data_garbage(scriptReturn, scriptContext, token.pos);
   SourceExpression::Pointer scriptExprRetn =
      create_branch_return(scriptExprData, scriptContext, token.pos);
   blocks->push_back(scriptExprRetn);

   if (scriptNumber < 0)
      ObjectData_Script::add
      (scriptNameObject, scriptLabel, scriptType, scriptFlags, scriptArgCount,
	  scriptContext);
   else
      ObjectData_Script::add
      (scriptNameObject, scriptLabel, scriptType, scriptFlags, scriptArgCount,
	  scriptContext, scriptNumber);

   return create_value_variable(scriptVariable, context, token.pos);
}

// EOF

