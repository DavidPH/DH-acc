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

/* SourceExpressionDS/make_expression_single.cpp
**
** Defines the SourceExpressionDS::make_expression_single function.
*/

#include "../SourceExpressionDS.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"

#include <sstream>



SourceExpression::Pointer SourceExpressionDS::make_expression_single(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceExpression::Pointer expr;
	SourceTokenC token(in->get());

	switch (token.getType())
	{
	case SourceTokenC::TT_CHARACTER:
		expr = create_value_char(token);
		break;

	case SourceTokenC::TT_FLOAT:
		expr = create_value_real(token);
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

	{	// Check for type.
		VariableType const * type(context->getVariableTypeNull(token.getData()));

		if (type)
		{
			expr = create_value_cast(make_expression_single(in, blocks, context), type, token.getPosition());
			break;
		}
	}

		// Check for label.
		if (in->peek().getType() == SourceTokenC::TT_OP_COLON && context->getAllowLabel())
		{
			in->get(SourceTokenC::TT_OP_COLON);
			expr = make_expression(in, blocks, context);
			expr->addLabel(context->getLabelGoto(token));
			break;
		}

		// Must be a variable.
		expr = create_value_variable(context->getVariable(token), token.getPosition());
		break;

	case SourceTokenC::TT_INTEGER:
		expr = create_value_int(token);
		break;

	case SourceTokenC::TT_STRING:
		expr = create_value_string(token);
		break;

	case SourceTokenC::TT_OP_AND:
		expr = create_unary_reference(make_expression_single(in, blocks, context), token.getPosition());
		break;

	case SourceTokenC::TT_OP_ASTERISK:
		expr = create_unary_dereference(make_expression_single(in, blocks, context), token.getPosition());
		break;

	case SourceTokenC::TT_OP_BRACE_O:
	{
		in->unget(token);
		std::vector<SourceExpression::Pointer> expressions;
		SourceContext blockContext(context, SourceContext::CT_BLOCK);
		make_expressions(in, &expressions, blocks, &blockContext);
		expr = create_value_block(expressions, token.getPosition());
		break;
	}

	case SourceTokenC::TT_OP_BRACKET_O:
	{
		in->unget(token);
		std::vector<SourceExpression::Pointer> expressions;
		make_expressions(in, &expressions, blocks, context);
		expr = create_value_block(expressions, token.getPosition());
		break;
	}

	case SourceTokenC::TT_OP_EXCLAMATION:
		expr = create_branch_not(make_expression_single(in, blocks, context), context, token.getPosition());
		break;

	case SourceTokenC::TT_OP_MINUS:
		expr = create_unary_sub(make_expression_single(in, blocks, context), token.getPosition());
		break;

	case SourceTokenC::TT_OP_MINUS2:
		expr = create_unary_dec_pre(make_expression_single(in, blocks, context), token.getPosition());
		break;

	case SourceTokenC::TT_OP_PARENTHESIS_C:
	case SourceTokenC::TT_OP_SEMICOLON:
		in->unget(token);
		return create_value_data(SourceContext::global_context.getVariableType(VariableType::VT_VOID), false, token.getPosition());

	case SourceTokenC::TT_OP_PARENTHESIS_O:
		expr = make_expression(in, blocks, context);
		in->get(SourceTokenC::TT_OP_PARENTHESIS_C);
		break;

	case SourceTokenC::TT_OP_PLUS:
		expr = make_expression_single(in, blocks, context);
		break;

	case SourceTokenC::TT_OP_PLUS2:
		expr = create_unary_inc_pre(make_expression_single(in, blocks, context), token.getPosition());
		break;

	case SourceTokenC::TT_OP_TILDE:
		expr = create_unary_not(make_expression_single(in, blocks, context), token.getPosition());
		break;

	default:
		in->unget(token);
		throw SourceException("unexpected token type (single)", token.getPosition(), "SourceExpressionDS");
	}

	while (true) switch (in->peek().getType())
	{
	case SourceTokenC::TT_OP_BRACKET_O:
		expr = create_binary_array(expr, make_expression(in, blocks, context), in->get(SourceTokenC::TT_OP_BRACKET_O).getPosition());
		in->get(SourceTokenC::TT_OP_BRACKET_C);
		break;

	case SourceTokenC::TT_OP_MINUS_GT:
		token = in->get(SourceTokenC::TT_OP_MINUS_GT);
		expr = create_value_member(create_unary_dereference(expr, token.getPosition()), in->get(SourceTokenC::TT_IDENTIFIER));
		break;

	case SourceTokenC::TT_OP_MINUS2:
		expr = create_unary_dec_suf(expr, in->get(SourceTokenC::TT_OP_MINUS2).getPosition());
		break;

	case SourceTokenC::TT_OP_PARENTHESIS_O:
	{
		token = in->get(SourceTokenC::TT_OP_PARENTHESIS_O);

		SourceContext contextCall(context, SourceContext::CT_BLOCK);

		std::vector<SourceExpression::Pointer> args;

		if (in->peek().getType() != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
		{
			args.push_back(make_expression(in, blocks, &contextCall));

			if (in->peek().getType() != SourceTokenC::TT_OP_COMMA)
				break;

			in->get(SourceTokenC::TT_OP_COMMA);
		}
		in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

		expr = create_branch_call(expr, args, &contextCall, token.getPosition());
	}
		break;

	case SourceTokenC::TT_OP_PERIOD:
		token = in->get(SourceTokenC::TT_OP_PERIOD);
		expr = create_value_member(expr, in->get(SourceTokenC::TT_IDENTIFIER));
		break;

	case SourceTokenC::TT_OP_PLUS2:
		expr = create_unary_inc_suf(expr, in->get(SourceTokenC::TT_OP_PLUS2).getPosition());
		break;

	default:
		return expr;
	}
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_asmfunc(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	std::string asmfuncName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	ObjectCode asmfuncOCode(ocode_get_code(in->get(SourceTokenC::TT_IDENTIFIER)));

	ObjectCode asmfuncOCodeImmediate(ocode_get_code(in->get(SourceTokenC::TT_IDENTIFIER)));

	std::vector<VariableType const *> asmfuncArgs;
	VariableType const * asmfuncReturn;
	make_expression_arglist(in, blocks, context, &asmfuncArgs, &asmfuncReturn);

	VariableType const * asmfuncVarType(context->getVariableType_asmfunc(asmfuncReturn, asmfuncArgs));

	SourceVariable::VariableData_AsmFunc asmfuncVarData = {asmfuncVarType, asmfuncOCode, asmfuncOCodeImmediate};

	SourceVariable asmfuncVariable(asmfuncName, asmfuncVarData, token.getPosition());

	context->addVariable(asmfuncVariable);
	return create_value_variable(asmfuncVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_break(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	return create_branch_break(context, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_case(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	context->setAllowLabel(false);
	bigsint value(make_expression(in, blocks, context)->makeObject()->resolveInt());
	context->setAllowLabel(true);

	in->get(SourceTokenC::TT_OP_COLON);

	SourceExpression::Pointer expr(make_expression(in, blocks, context));
	expr->addLabel(context->addLabelCase(value, token.getPosition()));
	return expr;
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_const(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	VariableType const * type(make_expression_type(in, blocks, context));
	std::string name(in->get(SourceTokenC::TT_IDENTIFIER).getData());
	in->get(SourceTokenC::TT_OP_EQUALS);
	SourceExpression::Pointer data(make_expression(in, blocks, context));

	SourceVariable var(name, type, data->makeObject(), token.getPosition());

	context->addVariable(var);

	return create_value_variable(var, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_continue(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	return create_branch_continue(context, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_default(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	in->get(SourceTokenC::TT_OP_COLON);

	SourceExpression::Pointer expr(make_expression(in, blocks, context));
	expr->addLabel(context->addLabelCaseDefault(token.getPosition()));
	return expr;
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_delay(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	return create_root_delay(make_expression(in, blocks, context), context, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_if(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceContext contextCondition(context, SourceContext::CT_BLOCK);
	SourceExpression::Pointer exprCondition(make_expression_single(in, blocks, &contextCondition));

	SourceContext contextIf(&contextCondition, SourceContext::CT_BLOCK);
	SourceExpression::Pointer exprIf(make_expression_single(in, blocks, &contextIf));

	if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER && in->peek().getData() == "else")
	{
		in->get();

		SourceContext contextElse(&contextCondition, SourceContext::CT_BLOCK);
		SourceExpression::Pointer exprElse(make_expression_single(in, blocks, &contextElse));

		return create_branch_if(exprCondition, exprIf, exprElse, context, token.getPosition());
	}
	else
	{
		return create_branch_if(exprCondition, exprIf, context, token.getPosition());
	}
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_out(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	return create_root_out(make_expression(in, blocks, context), token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_return(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	return create_branch_return(make_expression(in, blocks, context), context, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_script(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	// scriptContext
	SourceContext scriptContext(context, SourceContext::CT_SCRIPT);

	// scriptName
	std::string scriptName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	// scriptType
	ObjectData_Script::ScriptType scriptType;
	if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER)
	{
		SourceTokenC scriptTypeToken(in->get(SourceTokenC::TT_IDENTIFIER));
		scriptType = odata_get_ScriptType(scriptTypeToken);
	}
	else
	{
		scriptType = ObjectData_Script::ST_CLOSED;
	}

	// scriptFlags
	int scriptFlags(0);
	while (true)
	{
		if (in->peek().getType() != SourceTokenC::TT_IDENTIFIER)
			break;

		scriptFlags |= odata_get_ScriptFlag(in->get(SourceTokenC::TT_IDENTIFIER));
	}

	// scriptArgTypes/Names/Count scriptReturn
	std::vector<VariableType const *> scriptArgTypes;
	std::vector<std::string> scriptArgNames;
	int scriptArgCount;
	VariableType const * scriptReturn;
	make_expression_arglist(in, blocks, context, &scriptArgTypes, &scriptArgNames, &scriptArgCount, &scriptContext, &scriptReturn);

	// scriptNumber
	bigsint scriptNumber;
	if (in->peek().getType() == SourceTokenC::TT_OP_AT)
	{
		in->get(SourceTokenC::TT_OP_AT);
		scriptNumber = get_bigsint(in->get(SourceTokenC::TT_INTEGER));
	}
	else
	{
		scriptNumber = -1;
	}

	// scriptName special cases
	if (scriptName == "auto")
	{
		if (scriptNumber < 0)
			throw SourceException("script name auto requires explicit script number", token.getPosition(), "SourceExpressionDS");

		std::ostringstream oss;
		oss << "script" << scriptNumber;
		scriptName = oss.str();
	}

	// scriptLabel
	std::string scriptLabel(context->getLabel() + "script_" + (scriptName == "void" ? (context->makeLabel() + ObjectExpression::get_filename()) : scriptName));

	// scriptNameObject
	std::string scriptNameObject(scriptLabel + "_id");

	// scriptVarType
	VariableType const * scriptVarType(context->getVariableType_script(scriptReturn, scriptArgTypes));

	// scriptVarData
	SourceVariable::VariableData_Script scriptVarData = {scriptVarType, -1};

	// scriptExpression
	SourceExpression::Pointer scriptExpression(make_expression_single(in, blocks, &scriptContext));
	scriptExpression->addLabel(scriptLabel);
	blocks->push_back(scriptExpression);
	blocks->push_back(create_branch_return(create_value_data(scriptReturn, true, token.getPosition()), &scriptContext, token.getPosition()));

	// scriptVarCount
	int scriptVarCount(scriptContext.getLimit(SourceVariable::SC_REGISTER));

	// scriptVariable
	SourceVariable scriptVariable(scriptName == "void" ? (std::string)"" : scriptName, scriptVarData, token.getPosition(), scriptNameObject);

	context->addVariable(scriptVariable);

	if (scriptNumber < 0)
		ObjectExpression::add_script(scriptNameObject, scriptLabel, scriptType, scriptFlags, scriptArgCount, scriptVarCount);
	else
		ObjectExpression::add_script(scriptNameObject, scriptLabel, scriptType, scriptFlags, scriptArgCount, scriptVarCount, scriptNumber);

	return create_value_variable(scriptVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_sizeof(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	return create_value_int(make_expression_type(in, blocks, context)->size(), token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_type(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	in->unget(token);
	make_expression_type(in, blocks, context);

	return create_value_data(context->getVariableType(VariableType::VT_VOID), false, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_typedef(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	VariableType const * type(make_expression_type(in, blocks, context));
	context->getVariableType_typedef(in->get(SourceTokenC::TT_IDENTIFIER), type);

	return create_value_data(context->getVariableType(VariableType::VT_VOID), false, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_var(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceVariable::StorageClass sc(SourceVariable::get_StorageClass(in->get(SourceTokenC::TT_IDENTIFIER)));
	VariableType const * type(make_expression_type(in, blocks, context));
	std::string name(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	std::string nameObject;
	if (in->peek().getType() == SourceTokenC::TT_OP_AT)
	{
		in->get(SourceTokenC::TT_OP_AT);
		bigsint address = get_bigsint(in->get(SourceTokenC::TT_INTEGER));
		nameObject = context->makeNameObject(sc, type, name, address, token.getPosition());
	}
	else
	{
		nameObject = context->makeNameObject(sc, type, name, token.getPosition());
	}


	SourceVariable var(nameObject, name, sc, type, token.getPosition());

	context->addVariable(var);

	return create_value_variable(var, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_void(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	return create_value_cast(make_expression(in, blocks, context), context->getVariableType(VariableType::VT_VOID), token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_while(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceContext contextCondition(context, SourceContext::CT_BLOCK);
	SourceExpression::Pointer exprCondition(make_expression_single(in, blocks, &contextCondition));

	SourceContext contextWhile(&contextCondition, SourceContext::CT_LOOP);
	SourceExpression::Pointer exprWhile(make_expression_single(in, blocks, &contextWhile));

	return create_branch_while(exprCondition, exprWhile, &contextWhile, token.getPosition());
}



