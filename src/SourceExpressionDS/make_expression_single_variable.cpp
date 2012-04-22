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

#include "../ObjectExpression.hpp"
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
 SourceContext *context, SourcePosition const &pos, std::string const &nameSrc,
 VariableType *type, StorageClass sc, bool externDef, bool externVis)
{
   // Determine "name type".
   SourceContext::NameType nt;
   if (externDef)
      nt = SourceContext::NT_EXTERN;

   else if (externVis)
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
         // Only initialize static-duration storage-classes once.
      case SourceVariable::SC_STATIC:
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
         expr = exprSet;
         break;
      }
   }

   // Add variable to context.
   context->addVariable(var);

   return expr;
}

//
// make_var
//
static SourceExpression::Pointer make_var
(SourceTokenizerDS *in, SourceExpression::Vector *blocks,
 SourceContext *context, SourcePosition const &pos, VariableType *type,
 StorageClass sc, bool externDef, bool externVis)
{
   // Read source name.
   std::string nameSrc = in->get(SourceTokenC::TT_IDENTIFIER).data;

   return make_var(in, blocks, context, pos, nameSrc, type, sc, externDef, externVis);
}

//
// make_var
//
static SourceExpression::Pointer make_var
(SourceTokenizerDS *in, SourceExpression::Vector *blocks,
 SourceContext *context, SourcePosition const &pos, StorageClass sc,
 bool externDef, bool externVis)
{
   // Read variable type.
   VariableType::Reference type =
      SourceExpressionDS::make_expression_type(in, blocks, context);

   return make_var(in, blocks, context, pos, type, sc, externDef, externVis);
}

//
// make_var
//
static SourceExpression::Pointer make_var
(SourceTokenizerDS *in, SourceExpression::Vector *blocks,
 SourceContext *context, SourcePosition const &pos,
 bool externDef, bool externVis)
{
   // Read storage class.
   SourceTokenC const &scTok = in->get(SourceTokenC::TT_IDENTIFIER);
   SourceVariable::StorageClass sc =
      SourceVariable::get_StorageClass(scTok.data, scTok.pos);

   return make_var(in, blocks, context, pos, sc, externDef, externVis);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_extern_variable
//
SRCEXPDS_EXPRSINGLE_DEFN(extern_variable)
{
   return make_var(in, blocks, context, token.pos, true, true);
}

//
// SourceExpressionDS::make_expression_single_variable
//
SRCEXPDS_EXPRSINGLE_DEFN(variable)
{
   bool externVis = token.data == "__extvar";

   return make_var(in, blocks, context, token.pos, false, externVis);
}

// EOF

