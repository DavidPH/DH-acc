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
		return create_value_char(token);

	case SourceTokenC::TT_FLOAT:
		return create_value_real(token);

	case SourceTokenC::TT_IDENTIFIER:
	{	// Check for keyword.
		std::map<std::string, expression_single_handler>::iterator it(_expression_single_handlers.find(token.getData()));

		if (it != _expression_single_handlers.end())
			return it->second(in, token, blocks, context);
	}

	{	// Check for type.
		SourceVariable::VariableType const * type(SourceVariable::get_VariableType_null(token.getData()));

		if (type)
			return create_value_cast(make_expression_single(in, blocks, context), type, token.getPosition());
	}

		// Must be a variable.
		return create_value_variable(context->getVariable(token), token.getPosition());

	case SourceTokenC::TT_INTEGER:
		return create_value_int(token);

	case SourceTokenC::TT_STRING:
		return create_value_string(token);

	case SourceTokenC::TT_OP_AND:
		return create_unary_reference(make_expression_single(in, blocks, context), token.getPosition());

	case SourceTokenC::TT_OP_ASTERISK:
		return create_unary_dereference(make_expression_single(in, blocks, context), token.getPosition());

	case SourceTokenC::TT_OP_BRACE_O:
	{
		in->unget(token);
		std::vector<SourceExpression::Pointer> expressions;
		SourceContext blockContext(context, SourceContext::CT_BLOCK);
		make_expressions(in, &expressions, blocks, &blockContext);
		return create_value_block(expressions, token.getPosition());
	}

	case SourceTokenC::TT_OP_BRACKET_O:
	{
		in->unget(token);
		std::vector<SourceExpression::Pointer> expressions;
		make_expressions(in, &expressions, blocks, context);
		return create_value_block(expressions, token.getPosition());
	}

	case SourceTokenC::TT_OP_PARENTHESIS_O:
		expr = make_expression(in, blocks, context);
		in->get(SourceTokenC::TT_OP_PARENTHESIS_C);
		return expr;

	case SourceTokenC::TT_OP_SEMICOLON:
		in->unget(token);
		return create_value_data(SourceVariable::get_VariableType(SourceVariable::VT_VOID), false, token.getPosition());

	default:
		in->unget(token);
		throw SourceException("unexpected token type (single)", token.getPosition(), "SourceExpressionDS");
	}
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_acsfunc(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceContext acsfuncContext(context, SourceContext::CT_ACSFUNC);

	int acsfuncNumber(ObjectExpression::get_acsfunc_count());

	std::string acsfuncName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	std::string acsfuncLabel(context->getLabel() + "acsfunc_" + acsfuncName);

	std::vector<SourceVariable::VariableType const *> acsfuncArgTypes;
	std::vector<std::string> acsfuncArgNames;
	int acsfuncArgCount;
	SourceVariable::VariableType const * acsfuncReturn;
	make_expression_arglist(in, blocks, context, &acsfuncArgTypes, &acsfuncArgNames, &acsfuncArgCount, &acsfuncContext, &acsfuncReturn);

	SourceExpression::Pointer acsfuncExpression(make_expression_single(in, blocks, &acsfuncContext));
	acsfuncExpression->addLabel(acsfuncLabel);
	blocks->push_back(acsfuncExpression);
	blocks->push_back(create_branch_return(create_value_data(acsfuncReturn, true, token.getPosition()), &acsfuncContext, token.getPosition()));

	SourceVariable::VariableType const * acsfuncVarType(SourceVariable::get_VariableType_acsfunc(acsfuncReturn, acsfuncArgTypes));

	SourceVariable::VariableData_ACSFunc acsfuncVarData = {acsfuncVarType, acsfuncNumber};

	int acsfuncVarCount(acsfuncContext.getLimit(SourceVariable::SC_REGISTER));

	SourceVariable acsfuncVariable(acsfuncName, acsfuncVarData, token.getPosition());

	context->addVariable(acsfuncVariable);

	ObjectExpression::add_acsfunc(acsfuncLabel, acsfuncArgCount, acsfuncVarCount, acsfuncReturn->size());

	return create_value_variable(acsfuncVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_asmfunc(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	std::string asmfuncName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	ObjectToken::ObjectCode asmfuncOCode(ObjectToken::get_code(in->get(SourceTokenC::TT_IDENTIFIER)));

	ObjectToken::ObjectCode asmfuncOCodeImmediate(ObjectToken::get_code(in->get(SourceTokenC::TT_IDENTIFIER)));

	std::vector<SourceVariable::VariableType const *> asmfuncArgs;
	SourceVariable::VariableType const * asmfuncReturn;
	make_expression_arglist(in, blocks, context, &asmfuncArgs, &asmfuncReturn);

	SourceVariable::VariableType const * asmfuncVarType(SourceVariable::get_VariableType_asmfunc(asmfuncReturn, asmfuncArgs));

	SourceVariable::VariableData_AsmFunc asmfuncVarData = {asmfuncVarType, asmfuncOCode, asmfuncOCodeImmediate};

	SourceVariable asmfuncVariable(asmfuncName, asmfuncVarData, token.getPosition());

	context->addVariable(asmfuncVariable);
	return create_value_variable(asmfuncVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_const(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceVariable::VariableType const * type(make_expression_type(in, blocks, context));
	std::string name(in->get(SourceTokenC::TT_IDENTIFIER).getData());
	in->get(SourceTokenC::TT_OP_EQUALS);
	SourceExpression::Pointer data(make_expression(in, blocks, context));

	SourceVariable var(name, type, data->makeObject(), token.getPosition());

	context->addVariable(var);

	return create_value_variable(var, token.getPosition());
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
SourceExpression::Pointer SourceExpressionDS::make_expression_single_lnspec(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	std::string lnspecName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	ObjectExpression::int_t lnspecNumber(ObjectExpression::get_int(in->get(SourceTokenC::TT_INTEGER)));

	std::vector<SourceVariable::VariableType const *> lnspecArgs;
	SourceVariable::VariableType const * lnspecReturn;
	make_expression_arglist(in, blocks, context, &lnspecArgs, &lnspecReturn);

	SourceVariable::VariableType const * lnspecVarType(SourceVariable::get_VariableType_lnspec(lnspecReturn, lnspecArgs));

	SourceVariable::VariableData_LnSpec lnspecVarData = {lnspecVarType, lnspecNumber};

	SourceVariable lnspecVariable(lnspecName, lnspecVarData, token.getPosition());

	context->addVariable(lnspecVariable);
	return create_value_variable(lnspecVariable, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_native(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	std::string nativeName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	ObjectExpression::int_t nativeNumber(ObjectExpression::get_int(in->get(SourceTokenC::TT_INTEGER)));

	std::vector<SourceVariable::VariableType const *> nativeArgs;
	SourceVariable::VariableType const * nativeReturn;
	make_expression_arglist(in, blocks, context, &nativeArgs, &nativeReturn);

	SourceVariable::VariableType const * nativeVarType(SourceVariable::get_VariableType_native(nativeReturn, nativeArgs));

	SourceVariable::VariableData_Native nativeVarData = {nativeVarType, nativeNumber};

	SourceVariable nativeVariable(nativeName, nativeVarData, token.getPosition());

	context->addVariable(nativeVariable);
	return create_value_variable(nativeVariable, token.getPosition());
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
	ObjectExpression::ScriptType scriptType;
	if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER)
	{
		SourceTokenC scriptTypeToken(in->get(SourceTokenC::TT_IDENTIFIER));
		scriptType = ObjectExpression::get_ScriptType(scriptTypeToken.getData(), scriptTypeToken.getPosition());
	}
	else
	{
		scriptType = ObjectExpression::ST_CLOSED;
	}

	// scriptFlags
	int scriptFlags(0);
	while (true)
	{
		SourceTokenC token(in->get());

		if (token.getType() != SourceTokenC::TT_IDENTIFIER)
		{
			in->unget(token);
			break;
		}

		scriptFlags |= ObjectExpression::get_ScriptFlag(token.getData(), token.getPosition());
	}

	// scriptArgTypes/Names/Return
	std::vector<SourceVariable::VariableType const *> scriptArgTypes;
	std::vector<std::string> scriptArgNames;
	int scriptArgCount;
	SourceVariable::VariableType const * scriptReturn;
	make_expression_arglist(in, blocks, context, &scriptArgTypes, &scriptArgNames, &scriptArgCount, &scriptContext, &scriptReturn);

	// scriptNumber
	ObjectExpression::int_t scriptNumber;
	if (in->peek().getType() == SourceTokenC::TT_OP_AT)
	{
		in->get(SourceTokenC::TT_OP_AT);
		scriptNumber = ObjectExpression::get_int(in->get(SourceTokenC::TT_INTEGER));
		ObjectExpression::reserve_script_number(scriptNumber);
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
	std::string scriptLabel(context->getLabel() + "script_" + (scriptName == "void" ? context->makeLabel() : scriptName));

	// scriptNameObject
	std::string scriptNameObject(scriptLabel + "_id");

	// scriptVarType
	SourceVariable::VariableType const * scriptVarType(SourceVariable::get_VariableType_script(scriptReturn, scriptArgTypes));

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

	return create_value_data(SourceVariable::get_VariableType(SourceVariable::VT_VOID), false, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_typedef(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceVariable::VariableType const * type(make_expression_type(in, blocks, context));
	SourceVariable::add_typedef(in->get(SourceTokenC::TT_IDENTIFIER).getData(), type);

	return create_value_data(SourceVariable::get_VariableType(SourceVariable::VT_VOID), false, token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_var(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceVariable::StorageClass sc(SourceVariable::get_StorageClass(in->get(SourceTokenC::TT_IDENTIFIER)));
	SourceVariable::VariableType const * type(make_expression_type(in, blocks, context));
	std::string name(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	std::string nameObject;
	if (in->peek().getType() == SourceTokenC::TT_OP_AT)
	{
		in->get(SourceTokenC::TT_OP_AT);
		ObjectExpression::int_t address = ObjectExpression::get_int(in->get(SourceTokenC::TT_INTEGER));
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
	return create_value_cast(make_expression(in, blocks, context), SourceVariable::get_VariableType(SourceVariable::VT_VOID), token.getPosition());
}
SourceExpression::Pointer SourceExpressionDS::make_expression_single_while(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceContext contextCondition(context, SourceContext::CT_BLOCK);
	SourceExpression::Pointer exprCondition(make_expression_single(in, blocks, &contextCondition));

	SourceContext contextWhile(&contextCondition, SourceContext::CT_BLOCK);
	SourceExpression::Pointer exprWhile(make_expression_single(in, blocks, &contextWhile));

	return create_branch_while(exprCondition, exprWhile, context, token.getPosition());
}



