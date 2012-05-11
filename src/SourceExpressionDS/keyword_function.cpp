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
#include "../SourceVariable.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Static Prototypes                                                          |
//

static void mangle_types(VariableType::Vector const &types, std::string &name);


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

//
// make_func
//
static SourceExpression::Pointer make_func
(SourceTokenizerDS *in, SourceTokenC *tok, SourceExpression::Vector *blocks,
 SourceContext *context, SourceExpressionDS::LinkageSpecifier linkSpec,
 bool externDef)
{
   SourceExpressionDS::ArgList args;

   if (option_function_autoargs || target_type != TARGET_ZDoom)
      args.store = STORE_AUTO;
   else
      args.store = STORE_REGISTER;

   if (!externDef)
      args.context = SourceContext::create(context, SourceContext::CT_FUNCTION);

   SourceExpressionDS::make_arglist(in, blocks, context, &args);
   // Don't count automatic variable args.
   if (args.store == STORE_AUTO) args.count = 0;

   // funcLabel
   std::string funcLabel;
   switch (linkSpec)
   {
   case SourceExpressionDS::LS_INTERN:
      if (!args.name.empty())
      {
         funcLabel  = context->getLabel();
         funcLabel += args.name;
      }
      else
         funcLabel = context->makeLabel();

      mangle_types(args.types, funcLabel);

      break;

   case SourceExpressionDS::LS_ACS:
      if (!args.name.empty())
         funcLabel = args.name;
      else
         funcLabel = context->makeLabel();

      break;

   case SourceExpressionDS::LS_DS:
      if (!args.name.empty())
         funcLabel = args.name;
      else
         funcLabel = context->makeLabel();

      if (option_function_mangle_types)
         mangle_types(args.types, funcLabel);

      break;
   }

   // funcNameObject
   std::string funcNameObj = funcLabel + "_id";

   // __func__
   if (!externDef && option_string_func)
   {
      std::string funcFunc = args.name;

      if (option_function_mangle_types)
      {
         if (!args.types.empty())
         {
            funcFunc += '(';

            VariableType::Vector::iterator it = args.types.begin();
            funcFunc += make_string(*it);
            for (++it; it != args.types.end(); ++it)
            {
               funcFunc += ", ";
               funcFunc += make_string(*it);
            }

            funcFunc += ')';
         }
         else
            funcFunc += "(void)";

         funcFunc += " -> ";
         funcFunc += make_string(args.retn);
      }

      funcFunc = ObjectData_String::add(funcFunc);

      SourceVariable::Pointer funcFuncVar = SourceVariable::create_constant
         ("__func__", VariableType::get_bt_string(), funcFunc, tok->pos);

      args.context->addVar(funcFuncVar, false, false);
   }

   // funcVarType
   VariableType::Reference funcVarType =
      VariableType::get_bt_function(args.types, args.retn);

   // funcVar
   SourceVariable::Pointer funcVar;
   if (target_type != TARGET_ZDoom)
   {
      funcVar = SourceVariable::create_constant
         (args.name, funcVarType, funcLabel, tok->pos);
   }
   else
   {
      funcVar = SourceVariable::create_constant
         (args.name, funcVarType, funcNameObj, tok->pos);
   }

   // funcAdded
   bool funcAdded = ObjectData_Function::add
      (funcNameObj, funcLabel, args.count, args.retn->getSize(tok->pos),
       args.context);

   if (funcAdded)
      context->addFunction(funcVar);

   // funcExpr
   if (!externDef)
   {
      SourceExpression::Pointer funcExpr = SourceExpressionDS::
         make_prefix(in, blocks, args.context);

      SourceExpression::Pointer funcExprData = SourceExpression::
         create_value_data_garbage(args.retn, args.context, tok->pos);
      SourceExpression::Pointer funcExprRetn = SourceExpression::
         create_branch_return(funcExprData, args.context, tok->pos);

      funcExpr->addLabel(funcLabel);
      blocks->push_back(funcExpr);
      blocks->push_back(funcExprRetn);
   }

   return SourceExpression::create_value_variable(funcVar, context, tok->pos);
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
// SourceExpressionDS::make_extern_function
//
SRCEXPDS_EXTERN_DEFN(function)
{
   return make_func(in, tok, blocks, context, linkSpec, true);
}

//
// SourceExpressionDS::make_keyword_function
//
SRCEXPDS_KEYWORD_DEFN(function)
{
   LinkageSpecifier linkSpec;

   if (tok->data == "__function")
   {
      if (context == SourceContext::global_context)
         linkSpec = LS_DS;
      else
         linkSpec = LS_INTERN;
   }
   else if (tok->data == "__extfunc")
   {
      if (in->peekType(SourceTokenC::TT_STR))
         linkSpec = make_linkspec(in);
      else
         linkSpec = LS_DS;
   }
   else
      linkSpec = LS_INTERN;

   return make_func(in, tok, blocks, context, linkSpec, false);
}

// EOF

