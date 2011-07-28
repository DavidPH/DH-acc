/* Copyright (C) 2011 David Hill
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* SourceExpressionDS.cpp
**
** Defines the SourceExpressionDS methods.
*/

#include "SourceExpressionDS.hpp"

#include "SourceContext.hpp"
#include "SourceException.hpp"
#include "SourceTokenizerDS.hpp"

#include <sstream>



std::map<std::string, SourceExpressionDS::expression_single_handler> SourceExpressionDS::_expression_single_handlers;



SourceExpressionDS::SourceExpressionDS(SourcePosition const & position) : Super(position)
{

}

void SourceExpressionDS::init()
{
	_expression_single_handlers["acsfunc"] = make_expression_single_acsfunc;
	_expression_single_handlers["asmfunc"] = make_expression_single_asmfunc;
	_expression_single_handlers["const"]   = make_expression_single_const;
	_expression_single_handlers["delay"]   = make_expression_single_delay;
	_expression_single_handlers["if"]      = make_expression_single_if;
	_expression_single_handlers["lnspec"]  = make_expression_single_lnspec;
	_expression_single_handlers["native"]  = make_expression_single_native;
	_expression_single_handlers["out"]     = make_expression_single_out;
	_expression_single_handlers["return"]  = make_expression_single_return;
	_expression_single_handlers["script"]  = make_expression_single_script;
	_expression_single_handlers["struct"]  = make_expression_single_struct;
	_expression_single_handlers["typedef"] = make_expression_single_typedef;
	_expression_single_handlers["var"]     = make_expression_single_var;
	_expression_single_handlers["while"]   = make_expression_single_while;
}

SourceExpression::Pointer SourceExpressionDS::make_expression(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceExpression::Pointer expr(make_expression_single(in, blocks, context));

	while (true)
	{
		SourceTokenC token(in->get());

		switch (token.getType())
		{
		case SourceTokenC::TT_OP_ASTERISK:
			expr = create_binary_mul(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_BRACKET_C:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_BRACKET_O:
			expr = create_binary_array(expr, make_expression(in, blocks, context), token.getPosition());
			in->get(SourceTokenC::TT_OP_BRACKET_C);
			break;

		case SourceTokenC::TT_OP_COMMA:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_EQUALS:
			return create_binary_assign(expr, make_expression(in, blocks, context), token.getPosition());

		case SourceTokenC::TT_OP_MINUS:
			expr = create_binary_sub(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_PARENTHESIS_C:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_PARENTHESIS_O:
		{
			std::vector<SourceExpression::Pointer> args;

			if (in->peek().getType() != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
			{
				args.push_back(make_expression(in, blocks, context));

				SourceTokenC token(in->get());

				if (token.getType() != SourceTokenC::TT_OP_COMMA)
				{
					in->unget(token);
					break;
				}
			}
			in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

			expr = create_branch_call(expr, args, token.getPosition());
		}
			break;

		case SourceTokenC::TT_OP_PERCENT:
			expr = create_binary_mod(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_PERIOD:
			expr = create_value_member(expr, in->get(SourceTokenC::TT_IDENTIFIER));
			break;

		case SourceTokenC::TT_OP_PLUS:
			expr = create_binary_add(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_SEMICOLON:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_SLASH:
			expr = create_binary_div(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		default:
			in->unget(token);
			throw SourceException("unexpected token type", token.getPosition(), "SourceExpressionDS");
		}
	}
}

void SourceExpressionDS::make_expression_arglist(SourceTokenizerDS * in, std::vector<SourceVariable::VariableType const *> * argTypes, SourceVariable::VariableType const * * returnType)
{
	make_expression_arglist(in, argTypes, NULL, NULL, NULL, returnType);
}
void SourceExpressionDS::make_expression_arglist(SourceTokenizerDS * in, std::vector<SourceVariable::VariableType const *> * argTypes, std::vector<std::string> * argNames, int * argCount, SourceContext * argContext, SourceVariable::VariableType const * * returnType)
{
	in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
	if (in->peek().getType() != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
	{
		SourceVariable::StorageClass sc(SourceVariable::SC_REGISTER);

		argTypes->push_back(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));

		if (argNames)
		{
			argNames->push_back(in->get(SourceTokenC::TT_IDENTIFIER).getData());
		}
		else if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER)
		{
			in->get(SourceTokenC::TT_IDENTIFIER);
		}

		if (argContext && argNames)
		{
			int addr(argContext->getCount(sc));

			argContext->addVariable(SourceVariable(argNames->back(), argNames->back(), addr, sc, argTypes->back(), SourcePosition::none));
		}

		if (in->peek().getType() != SourceTokenC::TT_OP_COMMA)
			break;

		in->get(SourceTokenC::TT_OP_COMMA);
	}
	in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

	if (argCount)
	{
		if (argContext)
			*argCount = argContext->getLimit(SourceVariable::SC_REGISTER);
		else
		{
			*argCount = 0;

			for (size_t i(0); i < argTypes->size(); ++i)
				*argCount += (*argTypes)[i]->size();
		}
	}

	if (returnType)
	{
		in->get(SourceTokenC::TT_OP_MINUS_GT);

		*returnType = SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER));

		if (argContext) argContext->setReturnType(*returnType);
	}
}

SourceExpression::Pointer SourceExpressionDS::make_expressions(SourceTokenizerDS * in)
{
	SourcePosition position(in->peek().getPosition());
	std::vector<SourceExpression::Pointer> expressions;
	std::vector<SourceExpression::Pointer> blocks;
	SourceContext context(&SourceContext::global_context, SourceContext::CT_BLOCK);

	make_expressions(in, &expressions, &blocks, &context);

	for (size_t i(0); i < blocks.size(); ++i)
		expressions.push_back(blocks[i]);

	return create_value_block(expressions, position);
}
void SourceExpressionDS::make_expressions(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * expressions, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	bool brackets(in->peek().getType() == SourceTokenC::TT_OP_BRACKET_O);

	in->get(brackets ? SourceTokenC::TT_OP_BRACKET_O : SourceTokenC::TT_OP_BRACE_O);

	while (true)
	{
		if (in->peek().getType() == (brackets ? SourceTokenC::TT_OP_BRACKET_C : SourceTokenC::TT_OP_BRACE_C))
			break;

		expressions->push_back(make_expression(in, blocks, context));
		in->get(SourceTokenC::TT_OP_SEMICOLON);
	}

	in->get(brackets ? SourceTokenC::TT_OP_BRACKET_C : SourceTokenC::TT_OP_BRACE_C);
}



