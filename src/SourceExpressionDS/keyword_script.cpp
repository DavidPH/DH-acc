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
// is_script_flag
//
static bool is_script_flag(std::string const &data)
{
   if (data == "__net")        return true;
   if (data == "__clientside") return true;

   return false;
}

//
// is_script_type
//
static bool is_script_type(std::string const &data)
{
   if (data == "__closed")     return true;
   if (data == "__open")       return true;
   if (data == "__respawn")    return true;
   if (data == "__death")      return true;
   if (data == "__enter")      return true;
   if (data == "__lightning")  return true;
   if (data == "__unloading")  return true;
   if (data == "__disconnect") return true;
   if (data == "__return")     return true;

   return false;
}

//
// make_script_flag
//
static ObjectData_Script::ScriptFlag make_script_flag(SourceTokenizerDS *in)
{
   SourceTokenC tok = in->get(SourceTokenC::TT_NAM);

   if (tok.data == "__net")        return ObjectData_Script::SF_NET;
   if (tok.data == "__clientside") return ObjectData_Script::SF_CLIENTSIDE;

   ERROR(tok.pos, "invalid script-flag: %s", tok.data.c_str());
}

//
// make_script_type
//
static ObjectData_Script::ScriptType make_script_type(SourceTokenizerDS *in)
{
   SourceTokenC tok = in->get(SourceTokenC::TT_NAM);

   if (tok.data == "__closed")     return ObjectData_Script::ST_CLOSED;
   if (tok.data == "__open")       return ObjectData_Script::ST_OPEN;
   if (tok.data == "__respawn")    return ObjectData_Script::ST_RESPAWN;
   if (tok.data == "__death")      return ObjectData_Script::ST_DEATH;
   if (tok.data == "__enter")      return ObjectData_Script::ST_ENTER;
   if (tok.data == "__lightning")  return ObjectData_Script::ST_LIGHTNING;
   if (tok.data == "__unloading")  return ObjectData_Script::ST_UNLOADING;
   if (tok.data == "__disconnect") return ObjectData_Script::ST_DISCONNECT;
   if (tok.data == "__return")     return ObjectData_Script::ST_RETURN;

   ERROR(tok.pos, "invalid script-type: %s", tok.data.c_str());
}

//
// make_script
//
static SourceExpression::Pointer make_script
(SourceTokenizerDS *in, SourceTokenC const &tok,
 SourceExpression::Vector *blocks, SourceContext *context,
 SourceExpressionDS::LinkageSpecifier linkSpec, bool externDef)
{
   bool externVis = linkSpec != SourceExpressionDS::LS_INTERN;

   SourceExpressionDS::ArgList args;

   if (option_script_autoargs)
      args.store = STORE_AUTO;
   else
      args.store = STORE_REGISTER;

   if (!externDef)
      args.context = SourceContext::create(context, SourceContext::CT_SCRIPT);

   SourceExpressionDS::make_arglist(in, blocks, context, &args);

   // script-type
   ObjectData_Script::ScriptType scriptType;
   if (in->peekType(SourceTokenC::TT_NAM) && is_script_type(in->peek().data))
      scriptType = make_script_type(in);
   else
      scriptType = ObjectData_Script::ST_CLOSED;

   // script-flag-list
   bigsint scriptFlags = 0;
   while (in->peekType(SourceTokenC::TT_NAM) && is_script_flag(in->peek().data))
      scriptFlags |= make_script_flag(in);

   // scriptNumber
   ObjectExpression::Pointer scriptNumber;
   if (in->peekType(SourceTokenC::TT_AT))
   {
      in->get(SourceTokenC::TT_AT);
      scriptNumber = SourceExpressionDS::make_prefix(in, blocks, context)
         ->makeObject();
   }

   // __func__
   if (!externDef && option_string_func)
   {
      std::string scriptFunc = args.name;

      if (option_script_mangle_types)
      {
         if (!args.types.empty())
         {
            scriptFunc += '(';

            VariableType::Vector::iterator it = args.types.begin();
            scriptFunc += make_string(*it);
            for (++it; it != args.types.end(); ++it)
            {
               scriptFunc += ", ";
               scriptFunc += make_string(*it);
            }

            scriptFunc += ')';
         }
         else
            scriptFunc += "(void)";

         scriptFunc += " -> ";
         scriptFunc += make_string(args.retn);
      }

      scriptFunc = ObjectData_String::add(scriptFunc);

      SourceVariable::Pointer scriptFuncVar = SourceVariable::create_constant
         ("__func__", VariableType::get_bt_string(), scriptFunc, tok.pos);

      args.context->addVar(scriptFuncVar, false, false);
   }

   // scriptLabel
   std::string scriptLabel;
   switch (linkSpec)
   {
   case SourceExpressionDS::LS_INTERN:
      if (!args.name.empty())
      {
         scriptLabel  = context->getLabel();
         scriptLabel += args.name;
      }
      else
         scriptLabel = context->makeLabel();

      mangle_types(args.types, scriptLabel);

      break;

   case SourceExpressionDS::LS_ACS:
      if (!args.name.empty())
         scriptLabel = args.name;
      else
         scriptLabel = context->makeLabel();

      break;

   case SourceExpressionDS::LS_DS:
      if (!args.name.empty())
         scriptLabel = args.name;
      else
         scriptLabel = context->makeLabel();

      if (option_script_mangle_types)
         mangle_types(args.types, scriptLabel);

      break;
   }

   // scriptNameObj
   std::string scriptNameObj = scriptLabel + "_id";

   // scriptVarType
   VariableType::Reference scriptVarType =
      VariableType::get_bt_script(args.types, args.retn);

   // scriptVar
   SourceVariable::Pointer scriptVar = SourceVariable::create_constant
      (args.name, scriptVarType, scriptNameObj, tok.pos);

   // scriptAdded
   bool scriptAdded;
   if (scriptNumber)
      scriptAdded = ObjectData_Script::add
      (scriptNameObj, scriptLabel, scriptType, scriptFlags, args.count,
       args.context, externVis, scriptNumber->resolveInt());
   else
      scriptAdded = ObjectData_Script::add
      (scriptNameObj, scriptLabel, scriptType, scriptFlags, args.count,
       args.context, externVis);

   if (scriptAdded)
      context->addFunction(scriptVar);

   // scriptExpr
   if (!externDef)
   {
      SourceExpression::Pointer scriptExprRoot = SourceExpression::
         create_root_script(scriptVarType, args.context, tok.pos);

      SourceExpression::Pointer scriptExpr = SourceExpressionDS::
         make_prefix(in, blocks, args.context);

      SourceExpression::Pointer scriptExprData = SourceExpression::
         create_value_data_garbage(args.retn, args.context, tok.pos);
      SourceExpression::Pointer scriptExprRetn = SourceExpression::
         create_branch_return(scriptExprData, args.context, tok.pos);

      scriptExprRoot->addLabel(scriptLabel);
      blocks->push_back(scriptExprRoot);
      blocks->push_back(scriptExpr);
      blocks->push_back(scriptExprRetn);
   }

   return SourceExpression::create_value_variable(scriptVar, context, tok.pos);
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
// SourceExpressionDS::make_extern_script
//
SRCEXPDS_EXTERN_DEFN(script)
{
   return make_script(in, tok, blocks, context, linkSpec, true);
}

//
// SourceExpressionDS::make_keyword_script
//
SRCEXPDS_KEYWORD_DEFN(script)
{
   LinkageSpecifier linkSpec;

   if (tok.data == "__script")
   {
      if (context == SourceContext::global_context)
         linkSpec = LS_DS;
      else
         linkSpec = LS_INTERN;
   }
   else if (tok.data == "__extscript")
   {
      if (in->peekType(SourceTokenC::TT_STR))
         linkSpec = make_linkspec(in);
      else
         linkSpec = LS_DS;
   }
   else
      linkSpec = LS_INTERN;

   return make_script(in, tok, blocks, context, linkSpec, false);
}

// EOF

