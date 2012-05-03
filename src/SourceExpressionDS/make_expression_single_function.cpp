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
(SourceTokenizerDS *in, SourceTokenC const &token,
 SourceExpression::Vector *blocks, SourceContext *context,
 SourceExpressionDS::LinkageSpecifier linkSpec, bool externDef)
{
   // funcArgClass
   SourceVariable::StorageClass funcArgClass;

   if (option_function_autoargs || target_type != TARGET_ZDoom)
      funcArgClass = SourceVariable::SC_AUTO;
   else
      funcArgClass = SourceVariable::SC_REGISTER;

   // funcContext
   SourceContext::Pointer funcContext;
   if (!externDef)
      funcContext = SourceContext::create(context, SourceContext::CT_FUNCTION);

   // funcNameSrc
   std::string funcNameSrc = in->get(SourceTokenC::TT_IDENTIFIER).data;

   // funcArgTypes/Names/Count funcReturn
   VariableType::Vector funcArgTypes;
   VariableType::VecStr funcArgNames;
   int funcArgCount;
   VariableType::Pointer funcReturn;
   SourceExpressionDS::make_expression_arglist
   (in, blocks, context, &funcArgTypes, &funcArgNames, &funcArgCount,
    funcContext, &funcReturn, funcArgClass);
   // Don't count automatic variable args.
   if (option_function_autoargs) funcArgCount = 0;

   // funcLabel
   std::string funcLabel;
   switch (linkSpec)
   {
   case SourceExpressionDS::LS_INTERN:
      funcLabel  = context->getLabel();
      funcLabel += funcNameSrc;
      mangle_types(funcArgTypes, funcLabel);
      break;

   case SourceExpressionDS::LS_ACS:
      funcLabel  = funcNameSrc;
      break;

   case SourceExpressionDS::LS_DS:
      funcLabel  = funcNameSrc;
      if (option_function_mangle_types)
         mangle_types(funcArgTypes, funcLabel);
      break;
   }

   // funcNameObject
   std::string funcNameObj = funcLabel + "_id";

   // __func__
   if (!externDef && option_string_func)
   {
      std::string funcFunc = funcNameSrc;

      if (option_function_mangle_types)
      {
         if (!funcArgTypes.empty())
         {
            funcFunc += '(';

            VariableType::Vector::iterator it = funcArgTypes.begin();
            funcFunc += make_string(*it);
            for (++it; it != funcArgTypes.end(); ++it)
            {
               funcFunc += ", ";
               funcFunc += make_string(*it);
            }

            funcFunc += ')';
         }
         else
            funcFunc += "(void)";

         funcFunc += " -> ";
         funcFunc += make_string(funcReturn);
      }

      funcFunc = ObjectData_String::add(funcFunc);

      SourceVariable::Pointer funcFuncVar = SourceVariable::create_constant
         ("__func__", VariableType::get_bt_string(), funcFunc, token.pos);

      funcContext->addVar(funcFuncVar, false, false);
   }

   // funcVarType
   VariableType::Reference funcVarType =
      VariableType::get_bt_function(funcArgTypes, funcReturn);

   // funcVar
   SourceVariable::Pointer funcVar;
   if (target_type != TARGET_ZDoom)
   {
      funcVar = SourceVariable::create_constant
         (funcNameSrc, funcVarType, funcLabel, token.pos);
   }
   else
   {
      funcVar = SourceVariable::create_constant
         (funcNameSrc, funcVarType, funcNameObj, token.pos);
   }

   // funcAdded
   bool funcAdded = ObjectData_Function::add
      (funcNameObj, funcLabel, funcArgCount, funcReturn->getSize(token.pos),
       funcContext);

   if (funcAdded)
      context->addFunction(funcVar);

   // funcExpr
   if (!externDef)
   {
      SourceExpression::Pointer funcExpr = SourceExpressionDS::
         make_expression_single(in, blocks, funcContext);

      SourceExpression::Pointer funcExprData = SourceExpression::
         create_value_data_garbage(funcReturn, funcContext, token.pos);
      SourceExpression::Pointer funcExprRetn = SourceExpression::
         create_branch_return(funcExprData, funcContext, token.pos);

      funcExpr->addLabel(funcLabel);
      blocks->push_back(funcExpr);
      blocks->push_back(funcExprRetn);
   }

   return SourceExpression::create_value_variable(funcVar, context, token.pos);
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
// SourceExpressionDS::make_expression_extern_function
//
SRCEXPDS_EXPREXTERN_DEFN(function)
{
   return make_func(in, token, blocks, context, linkSpec, true);
}

//
// SourceExpressionDS::make_expression_single_function
//
SRCEXPDS_EXPRSINGLE_DEFN(function)
{
   LinkageSpecifier linkSpec;

   if (token.data == "__extfunc")
   {
      if (in->peekType(SourceTokenC::TT_STRING))
         linkSpec = make_linkage_specifier(in);
      else
         linkSpec = LS_DS;
   }
   else
      linkSpec = LS_INTERN;

   return make_func(in, token, blocks, context, linkSpec, false);
}

// EOF

