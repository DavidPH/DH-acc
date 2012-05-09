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
// DS handling of variable declaration.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../ObjectData.hpp"
#include "../ObjectExpression.hpp"
#include "../ost_type.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../SourceVariable.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

struct StoreData
{
   StoreType type;
   ObjectExpression::Pointer area;
};


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// make_var
//
static SourceExpression::Pointer make_var
(SourceTokenizerDS *in, SourceExpression::Vector *blocks,
 SourceContext *context, SourceExpressionDS::LinkageSpecifier linkSpec,
 SourcePosition const &pos, std::string const &nameSrc, VariableType *type,
 StoreData store, bool externDef)
{
   bool externVis = linkSpec != SourceExpressionDS::LS_INTERN;

   // Generate object name.
   std::string nameObj;
   switch (linkSpec)
   {
   case SourceExpressionDS::LS_INTERN:
      nameObj  = context->getLabel();
      nameObj += nameSrc;
      break;

   case SourceExpressionDS::LS_ACS:
   case SourceExpressionDS::LS_DS:
      nameObj  = nameSrc;
      break;
   }

   // Generate variable.
   SourceVariable::Pointer var =
      SourceVariable::create_variable(nameSrc, type, nameObj, store.type, pos);

   // Add variable to context.
   switch (store.type)
   {
   case STORE_MAPARRAY:
   case STORE_WORLDARRAY:
   case STORE_GLOBALARRAY:
      if (store.area)
         context->addVar(var, externDef, externVis, store.area->resolveInt());
      else
         context->addVar(var, externDef, externVis);

      if (in->peekType(SourceTokenC::TT_AT))
         ERROR_P("cannot have offset for store-type %s",
            make_string(store.type).c_str());

      break;


   default:
      if (store.area)
         ERROR_P("cannot have store-area for store-type %s",
            make_string(store.type).c_str());

      if (in->peekType(SourceTokenC::TT_AT))
      {
         in->get(SourceTokenC::TT_AT);

         bigsint addr = SourceExpressionDS::make_prefix(in, blocks, context)
            ->makeObject()->resolveInt();

         context->addVar(var, externDef, externVis, addr);
      }
      else
         context->addVar(var, externDef, externVis);

      break;
   }

   // Generate expression.
   SourceExpression::Pointer expr =
      SourceExpression::create_value_variable(var, context, pos);

   // Determine if metadata can be used.
   bool meta = true;
   switch (store.type)
   {
   case STORE_MAPARRAY:
      if (linkSpec == SourceExpressionDS::LS_ACS)
         ObjectData_Array::meta_map(nameObj, meta=false);
      break;

   case STORE_WORLDARRAY:
      if (linkSpec == SourceExpressionDS::LS_ACS)
         ObjectData_Array::meta_world(nameObj, meta=false);
      break;

   case STORE_GLOBALARRAY:
      if (linkSpec == SourceExpressionDS::LS_ACS)
         ObjectData_Array::meta_global(nameObj, meta=false);
      break;

   default:
      break;
   }

   // Variable initialization. (But not for external declaration.)
   if (!externDef && in->peekType(SourceTokenC::TT_EQUALS))
   {
      static VariableType::Reference const initType =
         VariableType::get_bt_boolhard();

      SourceExpression::Pointer initSrc, initExpr;
      ObjectExpression::Pointer initObj;

      in->get(SourceTokenC::TT_EQUALS);
      initSrc = SourceExpressionDS::make_assignment(in, blocks, context);

      if (initSrc->canMakeObject())
         initObj = initSrc->makeObject();

      // Generate expression to set variable.
      SourceExpression::Pointer exprSet = SourceExpression::
         create_binary_assign_const(expr, initSrc, context, pos);

      switch (store.type)
      {
      case STORE_MAPREGISTER:
         if (!initObj || target_type != TARGET_ZDoom)
            goto case_static;

         ObjectData_Register::ini_map(nameObj, initObj);
         break;

      case STORE_MAPARRAY:
         if (!initObj || target_type != TARGET_ZDoom)
            goto case_array;

         if (ObjectData_Array::ini_map(nameObj, initObj))
            break;

      case STORE_WORLDARRAY:
      case STORE_GLOBALARRAY:
      case_array:
         if (!meta)
            goto case_set;

         // Generate expression. *(bool __maparray(...) *)0
         initExpr = SourceExpression::create_value_int(0, context, pos);
         initExpr = SourceExpression::create_value_cast_explicit
            (initExpr, initType->setStorage(store.type, nameObj)->getPointer(),
             context, pos);
         initExpr = SourceExpression::create_unary_dereference
            (initExpr, context, pos);

         goto case_init;

         // Only initialize static-duration storage-classes once.
      case STORE_STATIC:
      case_static:
      {
         // Generate source/object name
         std::string initNameSrc = nameSrc + "$init";
         std::string initNameObj = context->getLabel() + initNameSrc;

         // Generate variable.
         SourceVariable::Pointer initVar = SourceVariable::create_variable
            (initNameSrc, initType, initNameObj, store.type, pos);
         context->addVar(initVar, false, false);

         // Generate expression.
         initExpr = SourceExpression::
            create_value_variable(initVar, context, pos);
      }
         goto case_init;

      case_init:
      {
         // Generate expression of value if initialized.
         SourceExpression::Pointer initVal = SourceExpression::
            create_value_int(1, context, pos);

         // Generate expression to set flag.
         SourceExpression::Pointer initSet = SourceExpression::
            create_binary_assign(initExpr, initVal, context, pos);

         // Generate expression to check if initialized.
         SourceExpression::Pointer initChk = SourceExpression::
            create_branch_not(initExpr, context, pos);

         // Generate expression to set variable and flag.
         SourceExpression::Vector initBothVector;
         initBothVector.push_back(exprSet);
         initBothVector.push_back(initSet);
         SourceExpression::Pointer initBoth = SourceExpression::
            create_value_block(initBothVector, context, pos);

         // Generate expression to conditionally set variable and flag.
         expr = SourceExpression::
            create_branch_if(initChk, initBoth, context, pos);
      }
         break;

      default:
      case_set:
         expr = exprSet;
         break;
      }
   }

   return expr;
}

//
// make_var
//
static SourceExpression::Pointer make_var
(SourceTokenizerDS *in, SourceExpression::Vector *blocks,
 SourceContext *context, SourceExpressionDS::LinkageSpecifier linkSpec,
 SourcePosition const &pos, VariableType *type, StoreData store,
 bool externDef)
{
   // If not followed by an identifier, then don't try to read any names.
   // (This is needed for standalone struct definitions.)
   if (!in->peekType(SourceTokenC::TT_NAM))
      return SourceExpression::create_value_data
         (VariableType::get_bt_void(), context, pos);

   SourceExpression::Vector vars;

   // Read source name.
   std::string nameSrc = in->get(SourceTokenC::TT_NAM).data;
   vars.push_back(make_var
      (in, blocks, context, linkSpec, pos, nameSrc, type, store, externDef));

   // Read in any additional variables.
   while (in->peekType(SourceTokenC::TT_COMMA))
   {
      in->get(SourceTokenC::TT_COMMA);

      nameSrc = in->get(SourceTokenC::TT_NAM).data;
      vars.push_back(make_var
         (in, blocks, context, linkSpec, pos, nameSrc, type, store, externDef));
   }

   if (vars.size() == 1)
      return vars[0];
   else
      return SourceExpression::create_value_block(vars, context, pos);
}

//
// make_var
//
static SourceExpression::Pointer make_var
(SourceTokenizerDS *in, SourceExpression::Vector *blocks,
 SourceContext *context, SourceExpressionDS::LinkageSpecifier linkSpec,
 SourcePosition const &pos, StoreData store, bool externDef)
{
   // Read variable type.
   VariableType::Reference type = SourceExpressionDS::
      make_type(in, blocks, context);

   // STORE_CONST is used to signal automatic storetype selection.
   if (store.type == STORE_CONST)
   {
      if (context == SourceContext::global_context)
      {
         if (type->getBasicType() == VariableType::BT_ARRAY)
            store.type = store_staticarray();
         else
            store.type = store_staticregister();
      }
      else
      {
         if (type->getBasicType() == VariableType::BT_ARRAY)
            store.type = store_autoarray();
         else
            store.type = store_autoregister();
      }
   }

   return make_var(in, blocks, context, linkSpec, pos, type, store, externDef);
}

//
// make_var
//
static SourceExpression::Pointer make_var
(SourceTokenizerDS *in, SourceExpression::Vector *blocks,
 SourceContext *context, SourceExpressionDS::LinkageSpecifier linkSpec,
 bool linkCheck, SourcePosition const &pos, bool externDef)
{
   // Read storage class.
   StoreData store;
   store.type = SourceExpressionDS::make_store(in, blocks, context, &store.area);

   if (linkCheck && (store.type == STORE_AUTO || store.type == STORE_REGISTER))
      linkSpec = SourceExpressionDS::LS_INTERN;

   return make_var(in, blocks, context, linkSpec, pos, store, externDef);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_extern_variable
//
SRCEXPDS_EXTERN_DEFN(variable)
{
   return make_var(in, blocks, context, linkSpec, false, tok.pos, true);
}

//
// SourceExpressionDS::make_keyword_variable
//
SRCEXPDS_KEYWORD_DEFN(variable)
{
   bool linkCheck = false;
   LinkageSpecifier linkSpec;

   if (tok.data == "__variable")
   {
      if (context == SourceContext::global_context)
      {
         linkCheck = true;
         linkSpec = LS_DS;
      }
      else
         linkSpec = LS_INTERN;
   }
   else if (tok.data == "__extvar")
   {
      if (in->peekType(SourceTokenC::TT_STR))
         linkSpec = make_linkspec(in);
      else
         linkSpec = LS_DS;
   }
   else
      linkSpec = LS_INTERN;

   return make_var(in, blocks, context, linkSpec, linkCheck, tok.pos, false);
}

//
// SourceExpressionDS::make_keyword_variable_store
//
SRCEXPDS_KEYWORD_DEFN(variable_store)
{
   bool linkCheck = false;
   LinkageSpecifier linkSpec;

   in->unget(tok);

   if (context == SourceContext::global_context)
   {
      linkCheck = true;
      linkSpec = LS_DS;
   }
   else
      linkSpec = LS_INTERN;

   return make_var(in, blocks, context, linkSpec, linkCheck, tok.pos, false);
}

//
// SourceExpressionDS::make_keyword_variable_type
//
SRCEXPDS_KEYWORD_DEFN(variable_type)
{
   LinkageSpecifier linkSpec;
   StoreData store = {STORE_CONST, NULL};

   in->unget(tok);

   if (context == SourceContext::global_context)
      linkSpec = LS_DS;
   else
      linkSpec = LS_INTERN;

   return make_var(in, blocks, context, linkSpec, tok.pos, store, false);
}

// EOF

