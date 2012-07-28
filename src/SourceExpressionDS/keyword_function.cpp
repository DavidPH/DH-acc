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
#include "../SourceException.hpp"
#include "../SourceFunction.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerC.hpp"
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
(SourceTokenizerC *in, SourceTokenC *tok, SourceContext *context,
 LinkageSpecifier linkSpec, bool externDef)
{
   SourceExpressionDS::ArgList args;

   if (option_function_autoargs || target_type != TARGET_ZDoom)
      args.store = STORE_AUTO;
   else
      args.store = STORE_REGISTER;

   if (!externDef)
      args.context = SourceContext::create(context, SourceContext::CT_FUNCTION);

   SourceExpressionDS::make_arglist(in, context, &args);
   // Don't count automatic variable args.
   if (args.store == STORE_AUTO) args.count = 0;

   // Anonymous function.
   if(args.name.empty())
   {
      linkSpec = LINKAGE_INTERN;
      if(externDef) ERROR(tok->pos, "extern anonymous");
   }

   // funcNameObj
   std::string funcNameObj;
   switch (linkSpec)
   {
   case LINKAGE_INTERN:
      if(args.name.empty())
         funcNameObj = context->makeLabel();
      else
         funcNameObj = context->getLabel() + args.name;

      mangle_types(args.types, funcNameObj);

      break;

   case LINKAGE_ACS:
      funcNameObj = args.name;
      break;

   case LINKAGE_C:
      funcNameObj = "_" + args.name;
      break;

   case LINKAGE_CPP:
      funcNameObj = context->getLabelNamespace() + args.name;
      mangle_types(args.types, funcNameObj);
      break;

   case LINKAGE_DS:
      funcNameObj = context->getLabelNamespace() + args.name;

      if (option_function_mangle_types)
         mangle_types(args.types, funcNameObj);

      break;
   }

   // funcLabel
   std::string funcLabel = funcNameObj + "_label";

   // __func__
   if (!externDef && option_string_func)
   {
      std::string funcFunc = context->getLabelNamespace() + args.name;

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
      }

      funcFunc = ObjectData_String::add(funcFunc);

      SourceVariable::Pointer funcFuncVar = SourceVariable::create_constant
         ("__func__", VariableType::get_bt_str(), funcFunc, tok->pos);

      args.context->addVar(funcFuncVar, false, false);
   }

   // funcVarType
   VariableType::Reference funcVarType = VariableType::get_bt_fun(args.types, args.retn);

   // funcVar
   SourceVariable::Pointer funcVar = SourceVariable::
      create_constant(args.name, funcVarType, funcNameObj, tok->pos);

   // funcAdded
   ObjectData_Function::add(funcNameObj, funcLabel, args.count,
                            args.retn->getSize(tok->pos), args.context);

   SourceFunction::Reference func = SourceFunction::FindFunction(funcVar, args.args);

   context->addFunction(func);

   // funcExpr
   if(!externDef)
      func->setBody(SourceExpressionDS::make_prefix(in, args.context), tok->pos);

   return SourceExpression::create_value_function(func, context, tok->pos);
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
   return make_func(in, tok, context, linkSpec, true);
}

//
// SourceExpressionDS::make_keyword_function
//
SRCEXPDS_KEYWORD_DEFN(function)
{
   LinkageSpecifier linkSpec;

   if (tok->data == "__function")
   {
      if(context->getType() == SourceContext::CT_NAMESPACE)
         linkSpec = LINKAGE_DS;
      else
         linkSpec = LINKAGE_INTERN;
   }
   else if (tok->data == "__extfunc")
   {
      if (in->peekType(SourceTokenC::TT_STR))
         linkSpec = make_linkspec(in);
      else
         linkSpec = LINKAGE_DS;
   }
   else
      linkSpec = LINKAGE_INTERN;

   return make_func(in, tok, context, linkSpec, false);
}

// EOF

