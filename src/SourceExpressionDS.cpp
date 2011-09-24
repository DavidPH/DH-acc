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
#include "VariableType.hpp"



std::map<std::string, SourceExpressionDS::expression_single_handler> SourceExpressionDS::_expression_single_handlers;



SourceExpressionDS::SourceExpressionDS(SourcePosition const & position) : Super(position)
{

}

void SourceExpressionDS::init()
{
	_expression_single_handlers["asmfunc"]  = make_expression_single_asmfunc;
	_expression_single_handlers["break"]    = make_expression_single_break;
	_expression_single_handlers["case"]     = make_expression_single_case;
	_expression_single_handlers["const"]    = make_expression_single_const;
	_expression_single_handlers["continue"] = make_expression_single_continue;
	_expression_single_handlers["default"]  = make_expression_single_default;
	_expression_single_handlers["delay"]    = make_expression_single_delay;
	_expression_single_handlers["do"]       = make_expression_single_do;
	_expression_single_handlers["enum"]     = make_expression_single_type;
	_expression_single_handlers["extern"]   = make_expression_single_extern;
	_expression_single_handlers["for"]      = make_expression_single_for;
	_expression_single_handlers["function"] = make_expression_single_function;
	_expression_single_handlers["goto"]     = make_expression_single_goto;
	_expression_single_handlers["if"]       = make_expression_single_if;
	_expression_single_handlers["linespec"] = make_expression_single_linespec;
	_expression_single_handlers["native"]   = make_expression_single_native;
	_expression_single_handlers["out"]      = make_expression_single_out;
	_expression_single_handlers["return"]   = make_expression_single_return;
	_expression_single_handlers["script"]   = make_expression_single_script;
	_expression_single_handlers["sizeof"]   = make_expression_single_sizeof;
	_expression_single_handlers["struct"]   = make_expression_single_type;
	_expression_single_handlers["switch"]   = make_expression_single_switch;
	_expression_single_handlers["typedef"]  = make_expression_single_typedef;
	_expression_single_handlers["union"]    = make_expression_single_type;
	_expression_single_handlers["var"]      = make_expression_single_var;
	_expression_single_handlers["void"]     = make_expression_single_void;
	_expression_single_handlers["while"]    = make_expression_single_while;
}

bool SourceExpressionDS::is_expression_type(std::string const & data, SourceContext * context)
{
	if (data == "array") return true;
	if (data == "block") return true;
	if (data == "enum") return true;
	if (data == "script") return true;
	if (data == "struct") return true;
	if (data == "typeof") return true;
	if (data == "union") return true;

	return context->getVariableTypeNull(data);
}

SourceExpression::Pointer SourceExpressionDS::make_expression(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceExpression::Pointer expr(make_expression_single(in, blocks, context));

	while (true)
	{
		SourceTokenC token(in->get());

		switch (token.getType())
		{
		case SourceTokenC::TT_OP_AND:
			expr = create_binary_and(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_AND_EQUALS:
			expr = create_binary_assign(expr, create_binary_and(expr, make_expression(in, blocks, context), token.getPosition()), token.getPosition());
			break;

		case SourceTokenC::TT_OP_AND2:
			expr = create_branch_and(expr, make_expression_single(in, blocks, context), context, token.getPosition());
			break;

		case SourceTokenC::TT_OP_AND2_EQUALS:
			expr = create_binary_assign(expr, create_branch_and(expr, make_expression(in, blocks, context), context, token.getPosition()), token.getPosition());
			break;

		case SourceTokenC::TT_OP_ASTERISK:
			expr = create_binary_mul(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_ASTERISK_EQUALS:
			expr = create_binary_assign(expr, create_binary_mul(expr, make_expression(in, blocks, context), token.getPosition()), token.getPosition());
			break;

		case SourceTokenC::TT_OP_BRACKET_C:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_CARET:
			expr = create_binary_xor(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_CARET_EQUALS:
			expr = create_binary_assign(expr, create_binary_xor(expr, make_expression(in, blocks, context), token.getPosition()), token.getPosition());
			break;

		case SourceTokenC::TT_OP_CARET2:
			expr = create_branch_xor(expr, make_expression_single(in, blocks, context), context, token.getPosition());
			break;

		case SourceTokenC::TT_OP_CARET2_EQUALS:
			expr = create_binary_assign(expr, create_branch_xor(expr, make_expression(in, blocks, context), context, token.getPosition()), token.getPosition());
			break;

		case SourceTokenC::TT_OP_CMP_EQ:
			expr = create_binary_eq(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_CMP_GE:
			expr = create_binary_ge(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_CMP_GT:
			expr = create_binary_gt(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_CMP_GT2:
			expr = create_binary_shiftr(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_CMP_GT2_EQUALS:
			expr = create_binary_assign(expr, create_binary_shiftr(expr, make_expression(in, blocks, context), token.getPosition()), token.getPosition());
			break;

		case SourceTokenC::TT_OP_CMP_LE:
			expr = create_binary_le(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_CMP_LT:
			expr = create_binary_lt(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_CMP_LT2:
			expr = create_binary_shiftl(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_CMP_LT2_EQUALS:
			expr = create_binary_assign(expr, create_binary_shiftl(expr, make_expression(in, blocks, context), token.getPosition()), token.getPosition());
			break;

		case SourceTokenC::TT_OP_CMP_NE:
			expr = create_binary_ne(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_COLON:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_COMMA:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_EQUALS:
			return create_binary_assign(expr, make_expression(in, blocks, context), token.getPosition());

		case SourceTokenC::TT_OP_MINUS:
			expr = create_binary_sub(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_MINUS_EQUALS:
			expr = create_binary_assign(expr, create_binary_sub(expr, make_expression(in, blocks, context), token.getPosition()), token.getPosition());
			break;

		case SourceTokenC::TT_OP_PARENTHESIS_C:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_PERCENT:
			expr = create_binary_mod(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_PERCENT_EQUALS:
			expr = create_binary_assign(expr, create_binary_mod(expr, make_expression(in, blocks, context), token.getPosition()), token.getPosition());
			break;

		case SourceTokenC::TT_OP_PIPE:
			expr = create_binary_ior(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_PIPE_EQUALS:
			expr = create_binary_assign(expr, create_binary_ior(expr, make_expression(in, blocks, context), token.getPosition()), token.getPosition());
			break;

		case SourceTokenC::TT_OP_PIPE2:
			expr = create_branch_ior(expr, make_expression_single(in, blocks, context), context, token.getPosition());
			break;

		case SourceTokenC::TT_OP_PIPE2_EQUALS:
			expr = create_binary_assign(expr, create_branch_ior(expr, make_expression(in, blocks, context), context, token.getPosition()), token.getPosition());
			break;

		case SourceTokenC::TT_OP_PLUS:
			expr = create_binary_add(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_PLUS_EQUALS:
			expr = create_binary_assign(expr, create_binary_add(expr, make_expression(in, blocks, context), token.getPosition()), token.getPosition());
			break;

		case SourceTokenC::TT_OP_QUERY:
		{
			SourceContext contextIf(context, SourceContext::CT_BLOCK);
			SourceExpression::Pointer exprIf(make_expression_single(in, blocks, &contextIf));

			in->get(SourceTokenC::TT_OP_COLON);

			SourceContext contextElse(context, SourceContext::CT_BLOCK);
			SourceExpression::Pointer exprElse(make_expression_single(in, blocks, &contextElse));

			expr = create_branch_if(expr, exprIf, exprElse, context, token.getPosition());
		}
			break;

		case SourceTokenC::TT_OP_SEMICOLON:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_SLASH:
			expr = create_binary_div(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_SLASH_EQUALS:
			expr = create_binary_assign(expr, create_binary_div(expr, make_expression(in, blocks, context), token.getPosition()), token.getPosition());
			break;

		default:
			in->unget(token);
			throw SourceException("unexpected token type", token.getPosition(), "SourceExpressionDS");
		}
	}
}

void SourceExpressionDS::make_expression_arglist(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context, std::vector<VariableType const *> * argTypes, VariableType const * * returnType)
{
	make_expression_arglist(in, blocks, context, argTypes, NULL, NULL, NULL, returnType);
}
void SourceExpressionDS::make_expression_arglist(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context, std::vector<VariableType const *> * argTypes, std::vector<std::string> * argNames, int * argCount, SourceContext * argContext, VariableType const * * returnType)
{
	SourceVariable::StorageClass const sc(SourceVariable::SC_REGISTER);

	if (argCount) *argCount = 0;

	in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
	if (in->peek().getType() != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
	{
		VariableType const * argType(make_expression_type(in, blocks, context));
		if (argCount) *argCount += argType->size(SourcePosition::none);
		if (argTypes) argTypes->push_back(argType);

		std::string argName;
		if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER)
			argName = in->get(SourceTokenC::TT_IDENTIFIER).getData();
		if (argNames) argNames->push_back(argName);

		if (argContext)
		{
			std::string argNameObject(argContext->makeNameObject(sc, argType, argName, SourcePosition::none));
			SourceVariable::Pointer argVariable(SourceVariable::create_variable(argName, argType, argNameObject, sc, SourcePosition::none));
			argContext->addVariable(argVariable);
		}

		if (in->peek().getType() != SourceTokenC::TT_OP_COMMA)
			break;

		in->get(SourceTokenC::TT_OP_COMMA);
	}
	in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

	if (returnType)
	{
		in->get(SourceTokenC::TT_OP_MINUS_GT);

		*returnType = make_expression_type(in, blocks, context);

		if (argContext) argContext->setReturnType(*returnType);
	}
}

SourceExpression::Pointer SourceExpressionDS::make_expressions(SourceTokenizerDS * in)
{
	SourcePosition position(in->peek().getPosition());
	std::vector<SourceExpression::Pointer> expressions;
	std::vector<SourceExpression::Pointer> blocks;

	make_expressions(in, &expressions, &blocks, &SourceContext::global_context);

	expressions.push_back(create_branch_return(create_value_data(VariableType::get_vt_void(), true, SourcePosition::none), &SourceContext::global_context, SourcePosition::none));

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


