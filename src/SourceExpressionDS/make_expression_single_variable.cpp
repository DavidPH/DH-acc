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
   // Determine "name type".
   SourceContext::NameType nt;
   if (externDef)
      nt = SourceContext::NT_EXTERN;
   else if (linkSpec != SourceExpressionDS::LS_INTERN)
      nt = SourceContext::NT_EXTLOCAL;
   else
      nt = SourceContext::NT_LOCAL;

   // Generate object name.
   std::string nameObj;
   if (in->peekType(SourceTokenC::TT_OP_AT))
   {
      in->get(SourceTokenC::TT_OP_AT);
      bigsint addr =
         SourceExpressionDS::make_expression_single(in, blocks, context)
         ->makeObject()->resolveInt();
      nameObj = context->makeNameObject(nt, sc, type, nameSrc, addr, pos);
   }
   else
      nameObj = context->makeNameObject(nt, sc, type, nameSrc, pos);

   // Generate variable.
   SourceVariable::Pointer var =
      SourceVariable::create_variable(nameSrc, type, nameObj, sc, pos);

   // Add variable to context.
   context->addVariable(var);

   // Generate expression.
   SourceExpression::Pointer expr =
      SourceExpression::create_value_variable(var, context, pos);

   // Variable initialization. (But not for external declaration.)
   if (!externDef && in->peekType(SourceTokenC::TT_OP_EQUALS))
   {
      static SourceContext::NameType const initNT = SourceContext::NT_LOCAL;
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
         std::string initNameObj =
            context->makeNameObject(initNT, sc, initType, initNameSrc, pos);

         // Generate variable.
         SourceVariable::Pointer initVar = SourceVariable::
            create_variable(initNameSrc, initType, initNameObj, sc, pos);
         context->addVariable(initVar);

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
 SourcePosition const &pos, bool externDef)
{
   // Read storage class.
   SourceTokenC const &scTok = in->get(SourceTokenC::TT_IDENTIFIER);
   SourceVariable::StorageClass sc =
      SourceVariable::get_StorageClass(scTok.data, scTok.pos);

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
   return make_var(in, blocks, context, linkSpec, token.pos, true);
}

//
// SourceExpressionDS::make_expression_single_variable
//
SRCEXPDS_EXPRSINGLE_DEFN(variable)
{
   LinkageSpecifier linkSpec;

   if (token.data == "__extvar")
   {
      if (in->peekType(SourceTokenC::TT_STRING))
         linkSpec = make_linkage_specifier(in);
      else
         linkSpec = LS_DS;
   }
   else
      linkSpec = LS_INTERN;

   return make_var(in, blocks, context, linkSpec, token.pos, false);
}

// EOF

