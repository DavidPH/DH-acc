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
#include "../option.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../SourceVariable.hpp"
#include "../VariableType.hpp"

#include <sstream>


//----------------------------------------------------------------------------|
// Static Prototypes                                                          |
//

static void mangle_types(VariableType::Vector const &types, std::string &name);


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

extern bool option_script_mangle_types;
static option::option_dptr<bool> option_script_mangle_types_handle
('\0', "script-mangle-types", "features",
 "Enables script name mangling based on types.",
 "Enables script name mangling based on types. Without this, overloading "
 "scripts may not work. However, this mangling may be incompatible with other "
 "compilers. It also makes the external use of named scripts precarious.",
 &option_script_mangle_types);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern bool option_script_autoargs;
bool option_script_mangle_types;
extern bool option_string_func;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// make_script
//
static SourceExpression::Pointer make_script
(SourceTokenizerDS *in, SourceTokenC const &token,
 SourceExpression::Vector *blocks, SourceContext *context,
 SourceExpressionDS::LinkageSpecifier linkSpec, bool externDef)
{
   bool externVis = linkSpec != SourceExpressionDS::LS_INTERN;

   // scriptArgStore
   StoreType scriptArgStore;

   if (option_script_autoargs)
      scriptArgStore = STORE_AUTO;
   else
      scriptArgStore = STORE_REGISTER;

   // scriptContext
   SourceContext::Pointer scriptContext;
   if (!externDef)
      scriptContext = SourceContext::create(context, SourceContext::CT_SCRIPT);

   // scriptNameSrc
   std::string scriptNameSrc = in->get(SourceTokenC::TT_IDENTIFIER).data;

   // scriptType
   ObjectData_Script::ScriptType scriptType;
   if (in->peekType(SourceTokenC::TT_IDENTIFIER))
   {
      SourceTokenC scriptTypeTok = in->get(SourceTokenC::TT_IDENTIFIER);
      scriptType = ObjectData_Script::
         get_type(scriptTypeTok.data, scriptTypeTok.pos);
   }
   else
      scriptType = ObjectData_Script::ST_CLOSED;

   // scriptFlags
   bigsint scriptFlags = 0;
   while (in->peekType(SourceTokenC::TT_IDENTIFIER))
   {
      SourceTokenC scriptFlagTok = in->get(SourceTokenC::TT_IDENTIFIER);
      scriptFlags |= ObjectData_Script::
         get_flag(scriptFlagTok.data, scriptFlagTok.pos);
   }

   // scriptArgTypes/Names/Count scriptReturn
   VariableType::Vector scriptArgTypes;
   VariableType::VecStr scriptArgNames;
   int scriptArgCount;
   VariableType::Pointer scriptReturn;
   SourceExpressionDS::make_expression_arglist
      (in, blocks, context, &scriptArgTypes, &scriptArgNames, &scriptArgCount,
       scriptContext, &scriptReturn, scriptArgStore);

   // scriptNumber
   bigsint scriptNumber;
   if (in->peekType(SourceTokenC::TT_OP_AT))
   {
      in->get(SourceTokenC::TT_OP_AT);
      scriptNumber = SourceExpressionDS::
         make_expression_single(in, blocks, context)->makeObject()->resolveInt();
   }
   else
   {
      scriptNumber = -1;
   }

   // scriptNameSrc special-cases
   if (scriptNameSrc == "auto")
   {
      if (scriptNumber < 0)
         ERROR(token.pos, "name auto requires explicit number");

      std::ostringstream oss;
      oss << "script" << scriptNumber;
      scriptNameSrc = oss.str();
   }
   else if (scriptNameSrc == "void")
      scriptNameSrc = "";

   // __func__
   if (!externDef && option_string_func)
   {
      std::string scriptFunc = scriptNameSrc.empty() ? "void" : scriptNameSrc;

      if (option_script_mangle_types)
      {
         if (!scriptArgTypes.empty())
         {
            scriptFunc += '(';

            VariableType::Vector::iterator it = scriptArgTypes.begin();
            scriptFunc += make_string(*it);
            for (++it; it != scriptArgTypes.end(); ++it)
            {
               scriptFunc += ", ";
               scriptFunc += make_string(*it);
            }

            scriptFunc += ')';
         }
         else
            scriptFunc += "(void)";

         scriptFunc += " -> ";
         scriptFunc += make_string(scriptReturn);
      }

      scriptFunc = ObjectData_String::add(scriptFunc);

      SourceVariable::Pointer scriptFuncVar = SourceVariable::create_constant
         ("__func__", VariableType::get_bt_string(), scriptFunc, token.pos);

      scriptContext->addVar(scriptFuncVar, false, false);
   }

   // scriptLabel
   std::string scriptLabel;
   switch (linkSpec)
   {
   case SourceExpressionDS::LS_INTERN:
      scriptLabel  = context->getLabel();
      scriptLabel += scriptNameSrc;
      mangle_types(scriptArgTypes, scriptLabel);
      break;

   case SourceExpressionDS::LS_ACS:
      scriptLabel  = scriptNameSrc;
      break;

   case SourceExpressionDS::LS_DS:
      scriptLabel  = scriptNameSrc;
      if (option_script_mangle_types)
         mangle_types(scriptArgTypes, scriptLabel);
      break;
   }

   // scriptNameObj
   std::string scriptNameObj = scriptLabel + "_id";

   // scriptVarType
   VariableType::Reference scriptVarType =
      VariableType::get_bt_script(scriptArgTypes, scriptReturn);

   // scriptVar
   SourceVariable::Pointer scriptVar = SourceVariable::create_constant
      (scriptNameSrc, scriptVarType, scriptNameObj, token.pos);

   // scriptAdded
   bool scriptAdded;
   if (scriptNumber < 0)
      scriptAdded = ObjectData_Script::add
      (scriptNameObj, scriptLabel, scriptType, scriptFlags, scriptArgCount,
       scriptContext, externVis);
   else
      scriptAdded = ObjectData_Script::add
      (scriptNameObj, scriptLabel, scriptType, scriptFlags, scriptArgCount,
       scriptContext, externVis, scriptNumber);

   if (scriptAdded)
      context->addFunction(scriptVar);

   // scriptExpr
   if (!externDef)
   {
      SourceExpression::Pointer scriptExprRoot = SourceExpression::
         create_root_script(scriptVarType, scriptContext, token.pos);

      SourceExpression::Pointer scriptExpr = SourceExpressionDS::
         make_expression_single(in, blocks, scriptContext);

      SourceExpression::Pointer scriptExprData = SourceExpression::
         create_value_data_garbage(scriptReturn, scriptContext, token.pos);
      SourceExpression::Pointer scriptExprRetn = SourceExpression::
         create_branch_return(scriptExprData, scriptContext, token.pos);

      scriptExprRoot->addLabel(scriptLabel);
      blocks->push_back(scriptExprRoot);
      blocks->push_back(scriptExpr);
      blocks->push_back(scriptExprRetn);
   }

   return SourceExpression::create_value_variable(scriptVar, context, token.pos);
}

//
// mangle_types
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
// SourceExpressionDS::make_expression_extern_script
//
SRCEXPDS_EXPREXTERN_DEFN(script)
{
   return make_script(in, token, blocks, context, linkSpec, true);
}

//
// SourceExpressionDS::make_expression_single_script
//
SRCEXPDS_EXPRSINGLE_DEFN(script)
{
   LinkageSpecifier linkSpec;

   if (token.data == "__script")
   {
      if (context == SourceContext::global_context)
         linkSpec = LS_DS;
      else
         linkSpec = LS_INTERN;
   }
   else if (token.data == "__extscript")
   {
      if (in->peekType(SourceTokenC::TT_STRING))
         linkSpec = make_linkage_specifier(in);
      else
         linkSpec = LS_DS;
   }
   else
      linkSpec = LS_INTERN;

   return make_script(in, token, blocks, context, linkSpec, false);
}

// EOF

