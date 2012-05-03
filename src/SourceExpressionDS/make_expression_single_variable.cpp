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
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

typedef SourceVariable::StorageClass StorageClass;


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
 StorageClass sc, bool externDef)
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
      SourceVariable::create_variable(nameSrc, type, nameObj, sc, pos);

   // Add variable to context.
   if (in->peekType(SourceTokenC::TT_OP_AT))
   {
      in->get(SourceTokenC::TT_OP_AT);

      bigsint addr = SourceExpressionDS::make_expression_single
         (in, blocks, context)->makeObject()->resolveInt();

      context->addVar(var, externDef, externVis, addr);
   }
   else
      context->addVar(var, externDef, externVis);

   // Generate expression.
   SourceExpression::Pointer expr =
      SourceExpression::create_value_variable(var, context, pos);

   // Variable initialization. (But not for external declaration.)
   if (!externDef && in->peekType(SourceTokenC::TT_OP_EQUALS))
   {
      static VariableType::Reference const initType =
         VariableType::get_bt_boolhard();

      SourceExpression::Pointer initSrc;
      ObjectExpression::Pointer initObj;

      in->get(SourceTokenC::TT_OP_EQUALS);
      initSrc = SourceExpressionDS::make_expression(in, blocks, context);

      if (initSrc->canMakeObject())
         initObj = initSrc->makeObject();

      // Generate expression to set variable.
      SourceExpression::Pointer exprSet = SourceExpression::
            create_binary_assign_const(expr, initSrc, context, pos);

      switch (sc)
      {
      case SourceVariable::SC_REGISTER_MAP:
         if (!initObj || target_type != TARGET_ZDoom)
            goto case_static;

         ObjectData_Register::ini_map(nameObj, initObj);

         break;

      case SourceVariable::SC_REGISTERARRAY_MAP:
         if (!initObj || target_type != TARGET_ZDoom)
            goto case_set;

         if (!ObjectData_Array::ini_map(nameObj, initObj))
            goto case_set;

         break;

         // Only initialize static-duration storage-classes once.
      case SourceVariable::SC_STATIC:
      case_static:
      {
         // Generate source/object name
         std::string initNameSrc = nameSrc + "$init";
         std::string initNameObj = context->getLabel() + initNameSrc;

         // Generate variable.
         SourceVariable::Pointer initVar = SourceVariable::
            create_variable(initNameSrc, initType, initNameObj, sc, pos);
         context->addVar(initVar, false, false);

         // Generate expression.
         SourceExpression::Pointer initExpr = SourceExpression::
            create_value_variable(initVar, context, pos);

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
 SourcePosition const &pos, VariableType *type, StorageClass sc, bool externDef)
{
   // Read source name.
   std::string nameSrc = in->get(SourceTokenC::TT_IDENTIFIER).data;

   return make_var(in, blocks, context, linkSpec, pos, nameSrc, type, sc, externDef);
}

//
// make_var
//
static SourceExpression::Pointer make_var
(SourceTokenizerDS *in, SourceExpression::Vector *blocks,
 SourceContext *context, SourceExpressionDS::LinkageSpecifier linkSpec,
 SourcePosition const &pos, StorageClass sc, bool externDef)
{
   // Read variable type.
   VariableType::Reference type =
      SourceExpressionDS::make_expression_type(in, blocks, context);

   return make_var(in, blocks, context, linkSpec, pos, type, sc, externDef);
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
   SourceTokenC const &scTok = in->get(SourceTokenC::TT_IDENTIFIER);
   SourceVariable::StorageClass sc =
      SourceVariable::get_StorageClass(scTok.data, scTok.pos);

   if (linkCheck && (sc == SourceVariable::SC_AUTO ||
                     sc == SourceVariable::SC_REGISTER))
      linkSpec = SourceExpressionDS::LS_INTERN;

   return make_var(in, blocks, context, linkSpec, pos, sc, externDef);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_extern_variable
//
SRCEXPDS_EXPREXTERN_DEFN(variable)
{
   return make_var(in, blocks, context, linkSpec, false, token.pos, true);
}

//
// SourceExpressionDS::make_expression_single_variable
//
SRCEXPDS_EXPRSINGLE_DEFN(variable)
{
   bool linkCheck = false;
   LinkageSpecifier linkSpec;

   if (token.data == "__variable")
   {
      if (context == SourceContext::global_context)
      {
         linkCheck = true;
         linkSpec = LS_DS;
      }
      else
         linkSpec = LS_INTERN;
   }
   else if (token.data == "__extvar")
   {
      if (in->peekType(SourceTokenC::TT_STRING))
         linkSpec = make_linkage_specifier(in);
      else
         linkSpec = LS_DS;
   }
   else
      linkSpec = LS_INTERN;

   return make_var(in, blocks, context, linkSpec, linkCheck, token.pos, false);
}

// EOF

