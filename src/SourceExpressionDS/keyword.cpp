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
// DS handling of single sub-expressions.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../ObjectCode.hpp"
#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../SourceVariable.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::is_keyword
//
bool SourceExpressionDS::is_keyword(std::string const &data)
{
   return expr_keyword.find(data) != expr_keyword.end();
}

//
// SourceExpressionDS::make_keyword_break
//
SRCEXPDS_KEYWORD_DEFN(break)
{
   (void)in; (void)blocks;

   return create_branch_break(context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_cast
//
SRCEXPDS_KEYWORD_DEFN(cast)
{
   in->get(SourceTokenC::TT_CMP_LT);
   VariableType::Reference type = make_type(in, blocks, context);
   in->get(SourceTokenC::TT_CMP_GT);

   in->get(SourceTokenC::TT_PAREN_O);
   SourceExpression::Pointer expr = make_expression(in, blocks, context);
   in->get(SourceTokenC::TT_PAREN_C);

   if (tok.data == "const_cast")
      return create_value_cast_qualifier(expr, type, context, tok.pos);

   if (tok.data == "reinterpret_cast")
      return create_value_cast_reinterpret(expr, type, context, tok.pos);

   if (tok.data == "static_cast")
      return create_value_cast_static(expr, type, context, tok.pos);

   if (tok.data == "__force_cast")
      return create_value_cast_force(expr, type, context, tok.pos);

   ERROR(tok.pos, "unknown cast: %s", tok.data.c_str());
}

//
// SourceExpressionDS::make_keyword_constexpr
//
SRCEXPDS_KEYWORD_DEFN(constexpr)
{
   VariableType::Reference type = make_type(in, blocks, context);
   std::string name = in->get(SourceTokenC::TT_NAM).data;
   in->get(SourceTokenC::TT_EQUALS);
   SourceExpression::Pointer data = make_assignment(in, blocks, context);
   data = create_value_cast_implicit(data, type, context, tok.pos);

   SourceVariable::Pointer var = SourceVariable::create_constant
      (name, type, data->makeObject(), tok.pos);

   context->addVar(var, false, false);

   return create_value_variable(var, context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_continue
//
SRCEXPDS_KEYWORD_DEFN(continue)
{
   (void)in; (void)blocks;

   return create_branch_continue(context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_do
//
SRCEXPDS_KEYWORD_DEFN(do)
{
   SourceContext::Reference contextBody =
      SourceContext::create(context, SourceContext::CT_LOOP);
   SourceExpression::Pointer exprBody =
      make_prefix(in, blocks, contextBody);

   SourceTokenC tokenWhile = in->get(SourceTokenC::TT_NAM, "while");

   in->get(SourceTokenC::TT_PAREN_O);
   SourceContext::Reference contextCond =
      SourceContext::create(contextBody, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond =
      make_expression(in, blocks, contextCond);
   in->get(SourceTokenC::TT_PAREN_C);

   return create_branch_do(exprCond, exprBody, contextCond, tok.pos);
}

//
// SourceExpressionDS::make_keyword_for
//
SRCEXPDS_KEYWORD_DEFN(for)
{
   in->get(SourceTokenC::TT_PAREN_O);

   SourceContext::Reference contextInit =
      SourceContext::create(context, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprInit =
      make_expression(in, blocks, contextInit);

   in->get(SourceTokenC::TT_SEMICOLON);

   SourceContext::Reference contextCond =
      SourceContext::create(contextInit, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond =
      make_expression(in, blocks, contextCond);

   in->get(SourceTokenC::TT_SEMICOLON);

   SourceContext::Reference contextIter =
      SourceContext::create(contextCond, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprIter =
      make_expression(in, blocks, contextIter);

   in->get(SourceTokenC::TT_PAREN_C);

   SourceContext::Reference contextBody =
      SourceContext::create(contextCond, SourceContext::CT_LOOP);
   SourceExpression::Pointer exprBody =
      make_prefix(in, blocks, contextBody);

   return create_branch_for(exprCond, exprBody, exprIter, exprInit, contextBody, tok.pos);
}

//
// SourceExpressionDS::make_keyword_goto
//
SRCEXPDS_KEYWORD_DEFN(goto)
{
   SourceTokenC gotoToken = in->get(SourceTokenC::TT_NAM);

   std::string label;

   if (gotoToken.data == "case")
   {
      if (in->peekType(SourceTokenC::TT_NAM, "default"))
      {
         in->get(SourceTokenC::TT_NAM);
         label = context->getLabelCaseDefault(tok.pos);
      }
      else
         label = context->getLabelCase(make_expression(in, blocks, context)
            ->makeObject()->resolveInt(), tok.pos);
   }
   else
      label = context->getLabelGoto(gotoToken.data, gotoToken.pos);

   return create_branch_goto(label, context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_goto_dyn
//
SRCEXPDS_KEYWORD_DEFN(goto_dyn)
{
   return create_branch_goto(make_expression(in, blocks, context), context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_if
//
SRCEXPDS_KEYWORD_DEFN(if)
{
   in->get(SourceTokenC::TT_PAREN_O);
   SourceContext::Reference contextCond =
      SourceContext::create(context, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond =
     make_expression(in, blocks, contextCond);
   in->get(SourceTokenC::TT_PAREN_C);

   SourceContext::Reference contextBody =
      SourceContext::create(contextCond, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprBody =
      make_prefix(in, blocks, contextBody);

   if (in->peekType(SourceTokenC::TT_NAM, "else"))
   {
      in->get();

      SourceContext::Reference contextElse =
         SourceContext::create(contextCond, SourceContext::CT_BLOCK);
      SourceExpression::Pointer exprElse =
         make_prefix(in, blocks, contextElse);

      return create_branch_if(exprCond, exprBody, exprElse, context, tok.pos);
   }
   else
      return create_branch_if(exprCond, exprBody, context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_library
//
SRCEXPDS_KEYWORD_DEFN(library)
{
   (void)blocks; (void)context;

   in->get(SourceTokenC::TT_PAREN_O);
   ObjectExpression::set_library(in->get(SourceTokenC::TT_STR).data);
   in->get(SourceTokenC::TT_PAREN_C);

   return create_value_data(context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_linespec
//
SRCEXPDS_KEYWORD_DEFN(linespec)
{
   ArgList args; make_arglist(in, blocks, context, &args);

   in->get(SourceTokenC::TT_AT); ObjectExpression::Pointer obj =
      make_prefix(in, blocks, context)->makeObject();

   VariableType::Reference varType = tok.data == "__linespec"
      ? VariableType::get_bt_linespec(args.types, args.retn) : tok.data == "__native"
      ? VariableType::get_bt_native  (args.types, args.retn)
      : VariableType::get_bt_asmfunc (args.types, args.retn);

   SourceVariable::Pointer var = SourceVariable::create_constant
      (args.name, varType, obj, tok.pos);

   if (!args.name.empty()) context->addFunction(var);
   return create_value_variable(var, context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_ocode
//
SRCEXPDS_KEYWORD_DEFN(ocode)
{
   (void)blocks;

   ObjectExpression::Pointer ocodeObj;
   SourceVariable::Pointer ocodeVar;
   VariableType::Reference ocodeTyp = VariableType::get_bt_void();
   SourceTokenC ocodeTok;
   ObjectCodeSet ocode;

   in->get(SourceTokenC::TT_PAREN_O);

   ocodeTok = in->get(SourceTokenC::TT_NAM);
   ocode.ocode = ocode_get_code(ocodeTok.data, ocodeTok.pos);

   if (!in->peekType(SourceTokenC::TT_PAREN_C))
   {
      in->get(SourceTokenC::TT_COMMA);

      ocodeTok = in->get(SourceTokenC::TT_NAM);
      ocode.ocode_imm = ocode_get_code(ocodeTok.data, ocodeTok.pos);
   }

   in->get(SourceTokenC::TT_PAREN_C);

   ocodeObj = ObjectExpression::create_value_ocode(ocode, tok.pos);
   ocodeVar = SourceVariable::create_literal(ocodeTyp, ocodeObj, tok.pos);

   return create_value_variable(ocodeVar, context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_output
//
SRCEXPDS_KEYWORD_DEFN(output)
{
   return create_root_output(make_expression(in, blocks, context), context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_printf
//
SRCEXPDS_KEYWORD_DEFN(printf)
{
   std::string type;

   if (in->peekType(SourceTokenC::TT_CMP_LT))
   {
      in->get(SourceTokenC::TT_CMP_LT);
      type = in->get(SourceTokenC::TT_NAM).data;
      in->get(SourceTokenC::TT_CMP_GT);
   }

   Vector exprs;

   in->get(SourceTokenC::TT_PAREN_O);

   std::string format = in->get(SourceTokenC::TT_STR).data;

   while (in->peekType(SourceTokenC::TT_COMMA))
   {
      in->get(SourceTokenC::TT_COMMA);
      exprs.push_back(make_assignment(in, blocks, context));
   }

   in->get(SourceTokenC::TT_PAREN_C);

   return create_root_printf(type, format, exprs, context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_return
//
SRCEXPDS_KEYWORD_DEFN(return)
{
   return create_branch_return(make_expression(in, blocks, context), context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_sizeof
//
SRCEXPDS_KEYWORD_DEFN(sizeof)
{
   VariableType::Pointer type;

   if (in->peekType(SourceTokenC::TT_PAREN_O))
   {
      in->get(SourceTokenC::TT_PAREN_O);

      if (in->peekType(SourceTokenC::TT_NAM) &&
          is_type(in->peek().data, context))
         type = make_type(in, blocks, context);
      else
         type = make_expression(in, blocks, context)->getType();

      in->get(SourceTokenC::TT_PAREN_C);
   }
   else
      type = make_prefix(in, blocks, context)->getType();

   return create_value_uint(type->getSize(tok.pos), context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_switch
//
SRCEXPDS_KEYWORD_DEFN(switch)
{
   in->get(SourceTokenC::TT_PAREN_O);
   SourceContext::Reference contextCond =
      SourceContext::create(context, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond =
      make_expression(in, blocks, contextCond);
   in->get(SourceTokenC::TT_PAREN_C);

   SourceContext::Reference contextBody =
      SourceContext::create(contextCond, SourceContext::CT_SWITCH);
   SourceExpression::Pointer exprBody =
      make_prefix(in, blocks, contextBody);

   return create_branch_switch(exprCond, exprBody, contextBody, tok.pos);
}

//
// SourceExpressionDS::make_keyword_symbol
//
SRCEXPDS_KEYWORD_DEFN(symbol)
{
   in->get(SourceTokenC::TT_PAREN_O);
   VariableType::Reference type = make_type(in, blocks, context);
   in->get(SourceTokenC::TT_COMMA);
   std::string name = in->get(SourceTokenC::TT_STR).data;
   in->get(SourceTokenC::TT_PAREN_C);

   SourceVariable::Pointer var = SourceVariable::
      create_literal(type, name, tok.pos);

   return create_value_variable(var, context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_typestr
//
SRCEXPDS_KEYWORD_DEFN(typestr)
{
   VariableType::Reference type = make_type(in, blocks, context);

   return create_value_string(make_string(type), context, tok.pos);
}

//
// SourceExpressionDS::make_keyword_while
//
SRCEXPDS_KEYWORD_DEFN(while)
{
   in->get(SourceTokenC::TT_PAREN_O);
   SourceContext::Reference contextCond =
      SourceContext::create(context, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond =
      make_expression(in, blocks, contextCond);
   in->get(SourceTokenC::TT_PAREN_C);

   SourceContext::Reference contextBody =
      SourceContext::create(contextCond, SourceContext::CT_LOOP);
   SourceExpression::Pointer exprBody =
      make_prefix(in, blocks, contextBody);

   return create_branch_while(exprCond, exprBody, contextBody, tok.pos);
}

// EOF

