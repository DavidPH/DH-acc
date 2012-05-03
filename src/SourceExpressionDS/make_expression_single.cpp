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

#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single
//
SourceExpression::Pointer SourceExpressionDS::make_expression_single(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
   #define PASS_A context, token.pos

   SourceExpression::Pointer expr;
   SourceTokenC token = in->get();

   switch (token.type)
   {
   case SourceTokenC::TT_CHARACTER:
      expr = create_value_char(token.data, PASS_A);
      break;

	case SourceTokenC::TT_FLOAT:
      expr = create_value_real(token.data, PASS_A);
		break;

   case SourceTokenC::TT_IDENTIFIER:
   {  // Check for keyword.
      SingleMap::iterator it = expr_single.find(token.data);

      if (it != expr_single.end())
      {
         expr = it->second(in, token, blocks, context);
         break;
      }
   }

      // Check for type.
      if (is_expression_type(token.data, context))
      {
         expr = make_expression_single_variable_type(in, token, blocks, context);
         break;
      }

      // Check for label.
      if (in->peekType(SourceTokenC::TT_OP_COLON) && context->getAllowLabel())
      {
         in->get(SourceTokenC::TT_OP_COLON);
         expr = make_expression(in, blocks, context);
         expr->addLabel(context->getLabelGoto(token.data, token.pos));
         break;
      }

   {  // Check for function designator.
      int count = context->isFunction(token.data);
      if (count == 1)
      {
         SourceVariable::Pointer func =
            context->getFunction(token.data, token.pos);
         expr = create_value_variable(func, PASS_A);
         break;
      }
      else if (count)
      {
         expr = create_value_function(token.data, PASS_A);
         break;
      }
   }

      // Must be a variable.
      expr = create_value_variable(context->getVariable(token.data, token.pos), PASS_A);
      break;

   case SourceTokenC::TT_INTEGER:
      expr = create_value_integer(token.data, PASS_A);
      break;

   case SourceTokenC::TT_STRING:
      expr = create_value_string(token.data, PASS_A);
      break;

   case SourceTokenC::TT_OP_AND:
      expr = create_unary_reference(make_expression_single(in, blocks, context), PASS_A);
      break;

   case SourceTokenC::TT_OP_AND2:
   {
      SourceTokenC labelToken = in->get(SourceTokenC::TT_IDENTIFIER);

      std::string label;

      if (labelToken.data == "case")
      {
         if (in->peekType(SourceTokenC::TT_IDENTIFIER, "default"))
         {
            in->get(SourceTokenC::TT_IDENTIFIER);
            label = context->getLabelCaseDefault(token.pos);
         }
         else
         {
            label = context->getLabelCase(make_expression_single(in, blocks, context)->makeObject()->resolveInt(), token.pos);
         }
      }
      else
      {
         label = context->getLabelGoto(labelToken.data, labelToken.pos);
      }

      expr = create_value_variable(SourceVariable::create_literal(VariableType::get_bt_label(), label, labelToken.pos), PASS_A);
   }
      break;

   case SourceTokenC::TT_OP_ASTERISK:
      expr = create_unary_dereference(make_expression_single(in, blocks, context), PASS_A);
      break;

   case SourceTokenC::TT_OP_AT:
   {
      ObjectExpression::Pointer addr;

      expr = make_expression_single(in, blocks, context);

      VariableData::Pointer data = expr->getData();

      switch (data->type)
      {
      case VariableData::MT_AUTO:
      case VariableData::MT_POINTER:
      case VariableData::MT_STATIC:
         addr = ObjectExpression::create_value_int(option_addr_array, token.pos);
         break;

      case VariableData::MT_LITERAL:
      case VariableData::MT_NONE:
      case VariableData::MT_REGISTER:
      case VariableData::MT_STACK:
      case VariableData::MT_VOID:
         ERROR(token.pos, "invalid MT for @");

      case VariableData::MT_REGISTERARRAY:
         addr = data->address;
         break;
      }

      VariableType::Reference type = VariableType::get_bt_uint();
      SourceVariable::Pointer var  =
         SourceVariable::create_literal(type, addr, token.pos);

      expr = create_value_variable(var, PASS_A);
   }
      break;

   case SourceTokenC::TT_OP_BRACE_O:
   {
      Vector expressions;
      SourceContext::Reference blockContext =
         SourceContext::create(context, SourceContext::CT_BLOCK);
      make_expressions(in, &expressions, blocks, blockContext);
      in->get(SourceTokenC::TT_OP_BRACE_C);
      expr = create_value_block(expressions, blockContext, token.pos);
      break;
   }

   case SourceTokenC::TT_OP_BRACKET_O:
   {
      Vector expressions;
      make_expressions(in, &expressions, blocks, context);
      in->get(SourceTokenC::TT_OP_BRACKET_C);
      expr = create_value_block(expressions, PASS_A);
      break;
   }

   case SourceTokenC::TT_OP_EXCLAMATION:
      expr = create_branch_not(make_expression_single(in, blocks, context), PASS_A);
      break;

   case SourceTokenC::TT_OP_MINUS:
      expr = create_unary_sub(make_expression_single(in, blocks, context), PASS_A);
      break;

   case SourceTokenC::TT_OP_MINUS2:
      expr = create_unary_dec_pre(make_expression_single(in, blocks, context), PASS_A);
      break;

   case SourceTokenC::TT_OP_PARENTHESIS_C:
   case SourceTokenC::TT_OP_SEMICOLON:
      in->unget(token);
      return create_value_data(VariableType::get_bt_void(), PASS_A);

   case SourceTokenC::TT_OP_PARENTHESIS_O:
      if (in->peekType(SourceTokenC::TT_IDENTIFIER) && is_expression_type(in->peek().data, context))
      {
         VariableType::Reference type =
            make_expression_type(in, blocks, context);
         in->get(SourceTokenC::TT_OP_PARENTHESIS_C);
         expr = create_value_cast_explicit(make_expression_single(in, blocks, context), type, PASS_A);
      }
      else
      {
         expr = make_expression(in, blocks, context);
         in->get(SourceTokenC::TT_OP_PARENTHESIS_C);
      }
      break;

   case SourceTokenC::TT_OP_PLUS:
      expr = make_expression_single(in, blocks, context);
      break;

   case SourceTokenC::TT_OP_PLUS2:
      expr = create_unary_inc_pre(make_expression_single(in, blocks, context), PASS_A);
      break;

   case SourceTokenC::TT_OP_TILDE:
      expr = create_unary_not(make_expression_single(in, blocks, context), PASS_A);
      break;

   default:
      in->unget(token);
      ERROR(token.pos, "unexpected token type: %s", make_string(token.type).c_str());
   }

   // Suffixes.
   while (true) switch (in->peek().type)
   {
   case SourceTokenC::TT_OP_BRACKET_O:
      token = in->get(SourceTokenC::TT_OP_BRACKET_O);
      expr = create_binary_array(expr, make_expression(in, blocks, context), PASS_A);
      in->get(SourceTokenC::TT_OP_BRACKET_C);
      break;

   case SourceTokenC::TT_OP_MINUS_GT:
   {
      token = in->get(SourceTokenC::TT_OP_MINUS_GT);
      SourceTokenC const &tokenMember = in->get(SourceTokenC::TT_IDENTIFIER);
      expr = create_value_member(create_unary_dereference(expr, PASS_A), tokenMember.data, PASS_A);
   }
      break;

   case SourceTokenC::TT_OP_MINUS2:
      token = in->get(SourceTokenC::TT_OP_MINUS2);
      expr = create_unary_dec_suf(expr, PASS_A);
      break;

   case SourceTokenC::TT_OP_PARENTHESIS_O:
      token = in->get(SourceTokenC::TT_OP_PARENTHESIS_O);

      if (in->peekType(SourceTokenC::TT_IDENTIFIER) &&
          is_expression_type(in->peek().data, context))
      {
         VariableType::Vector types;

         if (in->peek().type != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
         {
            types.push_back(make_expression_type(in, blocks, context));

            if (!in->peekType(SourceTokenC::TT_OP_COMMA))
               break;

            in->get(SourceTokenC::TT_OP_COMMA);
         }
         in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

         if (types.size() == 1 && types[0] == VariableType::get_bt_void())
            types.pop_back();

         expr = expr->makeExpressionFunction(types);
      }
      else
      {
         SourceContext::Reference contextCall =
            SourceContext::create(context, SourceContext::CT_BLOCK);

         Vector args;
         VariableType::Vector types;

         if (in->peek().type != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
         {
            args.push_back(make_expression(in, blocks, contextCall));
            types.push_back(args.back()->getType()->getUnqualified());

            if (!in->peekType(SourceTokenC::TT_OP_COMMA))
               break;

            in->get(SourceTokenC::TT_OP_COMMA);
         }
         in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

         expr = expr->makeExpressionFunction(types);
         expr = create_branch_call(expr, args, contextCall, token.pos);
      }
      break;

   case SourceTokenC::TT_OP_PERIOD:
   {
      token = in->get(SourceTokenC::TT_OP_PERIOD);
      SourceTokenC const &tokenMember = in->get(SourceTokenC::TT_IDENTIFIER);
      expr = create_value_member(expr, tokenMember.data, PASS_A);
   }
      break;

   case SourceTokenC::TT_OP_PLUS2:
      token = in->get(SourceTokenC::TT_OP_PLUS2);
      expr = create_unary_inc_suf(expr, PASS_A);
      break;

   default:
      return expr;
   }

   #undef PASS_A
}

//
// SourceExpressionDS::make_expression_single_break
//
SRCEXPDS_EXPRSINGLE_DEFN(break)
{
   (void)in; (void)blocks;

   return create_branch_break(context, token.pos);
}

//
// SourceExpressionDS::make_expression_single_case
//
SRCEXPDS_EXPRSINGLE_DEFN(case)
{
   context->setAllowLabel(false);
   bigsint value = make_expression(in, blocks, context)->makeObject()->resolveInt();
   context->setAllowLabel(true);

   in->get(SourceTokenC::TT_OP_COLON);

   SourceExpression::Pointer expr = make_expression(in, blocks, context);
   expr->addLabel(context->addLabelCase(value, token.pos));
   return expr;
}

//
// SourceExpressionDS::make_expression_single_constexpr
//
SRCEXPDS_EXPRSINGLE_DEFN(constexpr)
{
   VariableType::Reference type = make_expression_type(in, blocks, context);
   std::string name = in->get(SourceTokenC::TT_IDENTIFIER).data;
   in->get(SourceTokenC::TT_OP_EQUALS);
   SourceExpression::Pointer data = make_expression(in, blocks, context);
   data = create_value_cast_implicit(data, type, context, token.pos);

   SourceVariable::Pointer var =
      SourceVariable::create_constant
      (name, type, data->makeObject(), token.pos);

   context->addVar(var, false, false);

   return create_value_variable(var, context, token.pos);
}

//
// SourceExpressionDS::make_expression_single_continue
//
SRCEXPDS_EXPRSINGLE_DEFN(continue)
{
   (void)in; (void)blocks;

   return create_branch_continue(context, token.pos);
}

//
// SourceExpressionDS::make_expression_single_default
//
SRCEXPDS_EXPRSINGLE_DEFN(default)
{
   in->get(SourceTokenC::TT_OP_COLON);

   SourceExpression::Pointer expr = make_expression(in, blocks, context);
   expr->addLabel(context->addLabelCaseDefault(token.pos));
   return expr;
}

//
// SourceExpressionDS::make_expression_single_delay
//
SRCEXPDS_EXPRSINGLE_DEFN(delay)
{
   return create_root_delay(make_expression(in, blocks, context), context, token.pos);
}

//
// SourceExpressionDS::make_expression_single_library
//
SRCEXPDS_EXPRSINGLE_DEFN(library)
{
   (void)blocks; (void)context;

   in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
   ObjectExpression::set_library(in->get(SourceTokenC::TT_STRING).data);
   in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

   return create_value_data(VariableType::get_bt_void(), context, token.pos);
}

//
// SourceExpressionDS::make_expression_single_output
//
SRCEXPDS_EXPRSINGLE_DEFN(output)
{
   return create_root_output(make_expression(in, blocks, context), context, token.pos);
}

//
// SourceExpressionDS::make_expression_single_return
//
SRCEXPDS_EXPRSINGLE_DEFN(return)
{
   return create_branch_return(make_expression(in, blocks, context), context, token.pos);
}

//
// SourceExpressionDS::make_expression_single_typedef
//
SRCEXPDS_EXPRSINGLE_DEFN(typedef)
{
   VariableType::Reference type = make_expression_type(in, blocks, context);
   SourceTokenC name = in->get(SourceTokenC::TT_IDENTIFIER);
   context->getVariableType_typedef(name.data, type, name.pos);

   return create_value_data(VariableType::get_bt_void(), context, token.pos);
}

//
// SourceExpressionDS::make_expression_single_typestr
//
SRCEXPDS_EXPRSINGLE_DEFN(typestr)
{
   VariableType::Reference type = make_expression_type(in, blocks, context);

   return create_value_string(make_string(type), context, token.pos);
}

//
// SourceExpressionDS::make_expression_single_void
//
SRCEXPDS_EXPRSINGLE_DEFN(void)
{
   return create_value_cast_explicit
          (make_expression(in, blocks, context), VariableType::get_bt_void(),
		 context, token.pos);
}

//
// SourceExpressionDS::make_expression_single_while
//
SRCEXPDS_EXPRSINGLE_DEFN(while)
{
   in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
   SourceContext::Reference contextCond =
      SourceContext::create(context, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond =
      make_expression(in, blocks, contextCond);
   in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

   SourceContext::Reference contextBody =
      SourceContext::create(contextCond, SourceContext::CT_LOOP);
   SourceExpression::Pointer exprBody =
      make_expression_single(in, blocks, contextBody);

   return create_branch_while(exprCond, exprBody, contextBody, token.pos);
}

// EOF

