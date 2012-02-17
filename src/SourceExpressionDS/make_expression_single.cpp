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
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single
//
SourceExpression::Pointer SourceExpressionDS::make_expression_single(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
   #define PASS_A context, token.getPosition()

	SourceExpression::Pointer expr;
	SourceTokenC token(in->get());

	switch (token.getType())
	{
	case SourceTokenC::TT_CHARACTER:
      expr = create_value_char(token.getData(), PASS_A);
		break;

	case SourceTokenC::TT_FLOAT:
      expr = create_value_real(token.getData(), PASS_A);
		break;

	case SourceTokenC::TT_IDENTIFIER:
	{	// Check for keyword.
		std::map<std::string, expression_single_handler>::iterator it(_expression_single_handlers.find(token.getData()));

		if (it != _expression_single_handlers.end())
		{
			expr = it->second(in, token, blocks, context);
			break;
		}
	}

		// Check for label.
		if (in->peek().getType() == SourceTokenC::TT_OP_COLON && context->getAllowLabel())
		{
			in->get(SourceTokenC::TT_OP_COLON);
			expr = make_expression(in, blocks, context);
         expr->addLabel(context->getLabelGoto(token.getData(), token.getPosition()));
			break;
		}

		// Must be a variable.
      expr = create_value_variable(context->getVariable(token.getData(), token.getPosition()), PASS_A);
		break;

	case SourceTokenC::TT_INTEGER:
      expr = create_value_int(token.getData(), PASS_A);
		break;

	case SourceTokenC::TT_STRING:
      expr = create_value_string(token.getData(), PASS_A);
		break;

	case SourceTokenC::TT_OP_AND:
      expr = create_unary_reference(make_expression_single(in, blocks, context), PASS_A);
		break;

	case SourceTokenC::TT_OP_AND2:
	{
		SourceTokenC labelToken(in->get(SourceTokenC::TT_IDENTIFIER));

		std::string label;

		if (labelToken.getData() == "case")
		{
			if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER && in->peek().getData() == "default")
			{
				in->get(SourceTokenC::TT_IDENTIFIER);
				label = context->getLabelCaseDefault(token.getPosition());
			}
			else
			{
				label = context->getLabelCase(make_expression_single(in, blocks, context)->makeObject()->resolveInt(), token.getPosition());
			}
		}
		else
		{
			label = context->getLabelGoto(labelToken.getData(), labelToken.getPosition());
		}

      expr = create_value_variable(SourceVariable::create_literal(VariableType::get_bt_label(), label, labelToken.getPosition()), PASS_A);
	}
		break;

	case SourceTokenC::TT_OP_ASTERISK:
      expr = create_unary_dereference(make_expression_single(in, blocks, context), PASS_A);
		break;

	case SourceTokenC::TT_OP_BRACE_O:
	{
		in->unget(token);
		std::vector<SourceExpression::Pointer> expressions;
		SourceContext::Reference blockContext = SourceContext::create(context, SourceContext::CT_BLOCK);
		make_expressions(in, &expressions, blocks, blockContext);
      expr = create_value_block(expressions, blockContext, token.getPosition());
		break;
	}

	case SourceTokenC::TT_OP_BRACKET_O:
	{
		in->unget(token);
		std::vector<SourceExpression::Pointer> expressions;
		make_expressions(in, &expressions, blocks, context);
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
		if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER && is_expression_type(in->peek().getData(), context))
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
		throw SourceException("unexpected token type: " + (std::string)make_string(token.getType()), token.getPosition(), "SourceExpressionDS::make_expression_single");
	}

	// Suffixes.
	while (true) switch (in->peek().getType())
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
      expr = create_value_member(create_unary_dereference(expr, PASS_A), tokenMember.getData(), PASS_A);
   }
		break;

	case SourceTokenC::TT_OP_MINUS2:
      token = in->get(SourceTokenC::TT_OP_MINUS2);
      expr = create_unary_dec_suf(expr, PASS_A);
		break;

	case SourceTokenC::TT_OP_PARENTHESIS_O:
	{
		token = in->get(SourceTokenC::TT_OP_PARENTHESIS_O);

		SourceContext::Reference contextCall = SourceContext::create(context, SourceContext::CT_BLOCK);

		std::vector<SourceExpression::Pointer> args;

		if (in->peek().getType() != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
		{
			args.push_back(make_expression(in, blocks, contextCall));

			if (in->peek().getType() != SourceTokenC::TT_OP_COMMA)
				break;

			in->get(SourceTokenC::TT_OP_COMMA);
		}
		in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

		expr = create_branch_call(expr, args, contextCall, token.getPosition());
	}
		break;

	case SourceTokenC::TT_OP_PERIOD:
   {
		token = in->get(SourceTokenC::TT_OP_PERIOD);
      SourceTokenC const &tokenMember = in->get(SourceTokenC::TT_IDENTIFIER);
      expr = create_value_member(expr, tokenMember.getData(), PASS_A);
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

	return create_branch_break(context, token.getPosition());
}

//
// SourceExpressionDS::make_expression_single_case
//
SRCEXPDS_EXPRSINGLE_DEFN(case)
{
	context->setAllowLabel(false);
	bigsint value(make_expression(in, blocks, context)->makeObject()->resolveInt());
	context->setAllowLabel(true);

	in->get(SourceTokenC::TT_OP_COLON);

	SourceExpression::Pointer expr(make_expression(in, blocks, context));
	expr->addLabel(context->addLabelCase(value, token.getPosition()));
	return expr;
}

//
// SourceExpressionDS::make_expression_single_constexpr
//
SRCEXPDS_EXPRSINGLE_DEFN(constexpr)
{
   VariableType::Reference type = make_expression_type(in, blocks, context);
   std::string name = in->get(SourceTokenC::TT_IDENTIFIER).getData();
   in->get(SourceTokenC::TT_OP_EQUALS);
   SourceExpression::Pointer data = make_expression(in, blocks, context);

   SourceVariable::Pointer var =
      SourceVariable::create_constant
      (name, type, data->makeObject(), token.getPosition());

   context->addVariable(var);

   return create_value_variable(var, context, token.getPosition());
}

//
// SourceExpressionDS::make_expression_single_continue
//
SRCEXPDS_EXPRSINGLE_DEFN(continue)
{
	(void)in; (void)blocks;

	return create_branch_continue(context, token.getPosition());
}

//
// SourceExpressionDS::make_expression_single_default
//
SRCEXPDS_EXPRSINGLE_DEFN(default)
{
	in->get(SourceTokenC::TT_OP_COLON);

	SourceExpression::Pointer expr(make_expression(in, blocks, context));
	expr->addLabel(context->addLabelCaseDefault(token.getPosition()));
	return expr;
}

//
// SourceExpressionDS::make_expression_single_delay
//
SRCEXPDS_EXPRSINGLE_DEFN(delay)
{
	return create_root_delay(make_expression(in, blocks, context), context, token.getPosition());
}

//
// SourceExpressionDS::make_expression_single_library
//
SRCEXPDS_EXPRSINGLE_DEFN(library)
{
	(void)blocks; (void)context;

	in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
	ObjectExpression::set_library(in->get(SourceTokenC::TT_STRING).getData());
	in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

   return create_value_data
          (VariableType::get_bt_void(), context, token.getPosition());
}

//
// SourceExpressionDS::make_expression_single_output
//
SRCEXPDS_EXPRSINGLE_DEFN(output)
{
   return create_root_output(make_expression(in, blocks, context), context, token.getPosition());
}

//
// SourceExpressionDS::make_expression_single_return
//
SRCEXPDS_EXPRSINGLE_DEFN(return)
{
	return create_branch_return(make_expression(in, blocks, context), context, token.getPosition());
}

//
// SourceExpressionDS::make_expression_single_sizeof
//
SRCEXPDS_EXPRSINGLE_DEFN(sizeof)
{
	bool hasParentheses(in->peek().getType() == SourceTokenC::TT_OP_PARENTHESIS_O);

	if (hasParentheses) in->get(SourceTokenC::TT_OP_PARENTHESIS_O);

   bigsint size =
      make_expression_type(in, blocks, context)->getSize(token.getPosition());

	if (hasParentheses) in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

   return create_value_int(size, context, token.getPosition());
}

//
// SourceExpressionDS::make_expression_single_type
//
SRCEXPDS_EXPRSINGLE_DEFN(type)
{
	in->unget(token);
	make_expression_type(in, blocks, context);

   return create_value_data
          (VariableType::get_bt_void(), context, token.getPosition());
}

//
// SourceExpressionDS::make_expression_single_typedef
//
SRCEXPDS_EXPRSINGLE_DEFN(typedef)
{
   VariableType::Reference type = make_expression_type(in, blocks, context);
   SourceTokenC name = in->get(SourceTokenC::TT_IDENTIFIER);
   context->getVariableType_typedef(name.getData(), type, name.getPosition());

   return create_value_data
          (VariableType::get_bt_void(), context, token.getPosition());
}

//
// SourceExpressionDS::make_expression_single_void
//
SRCEXPDS_EXPRSINGLE_DEFN(void)
{
   return create_value_cast_explicit
          (make_expression(in, blocks, context), VariableType::get_bt_void(),
		 context, token.getPosition());
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

	return create_branch_while(exprCond, exprBody, contextBody, token.getPosition());
}

// EOF

