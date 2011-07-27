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



SourceExpressionDS SourceExpressionDS::make_expression_single(SourceTokenizerDS * in, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	SourceExpressionDS expr;
	SourceTokenC token(in->get());

	switch (token.getType())
	{
	case SourceTokenC::TT_CHARACTER:
		return make_expression_value_char(token);

	case SourceTokenC::TT_FLOAT:
		return make_expression_value_real(token);

	case SourceTokenC::TT_IDENTIFIER:
	{	// Check for keyword.
		std::map<std::string, expression_single_handler>::iterator it(_expression_single_handlers.find(token.getData()));

		if (it != _expression_single_handlers.end())
			return it->second(in, token, blocks, context);
	}

	{	// Check for type.
		SourceVariable::VariableType const * type(SourceVariable::get_VariableType_null(token.getData()));

		if (type)
			return make_expression_cast(make_expression_single(in, blocks, context), type, token.getPosition());
	}

		// Must be a variable.
		return make_expression_value_variable(context->getVariable(token), token.getPosition());

	case SourceTokenC::TT_INTEGER:
		return make_expression_value_int(token);

	case SourceTokenC::TT_STRING:
		return make_expression_value_string(token);

	case SourceTokenC::TT_OP_BRACE_O:
	{
		in->unget(token);
		std::vector<SourceExpressionDS> expressions;
		SourceContext blockContext(context, SourceContext::CT_BLOCK);
		make_expressions(in, &expressions, blocks, &blockContext);
		return make_expression_root_block(expressions, token.getPosition());
	}

	case SourceTokenC::TT_OP_BRACKET_O:
	{
		in->unget(token);
		std::vector<SourceExpressionDS> expressions;
		make_expressions(in, &expressions, blocks, context);
		return make_expression_root_block(expressions, token.getPosition());
	}

	case SourceTokenC::TT_OP_PARENTHESIS_O:
		expr = make_expression(in, blocks, context);
		in->get(SourceTokenC::TT_OP_PARENTHESIS_C);
		return expr;

	case SourceTokenC::TT_OP_SEMICOLON:
		in->unget(token);
		return NULL;

	default:
		in->unget(token);
		throw SourceException("unexpected token type (single)", token.getPosition(), "SourceExpressionDS");
	}
}
SourceExpressionDS SourceExpressionDS::make_expression_single_acsfunc(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	SourceContext acsfuncContext(context, SourceContext::CT_ACSFUNC);

	int acsfuncNumber(ObjectExpression::get_acsfunc_count());

	std::string acsfuncName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	std::string acsfuncLabel(context->getLabel() + "acsfunc_" + acsfuncName);

	std::vector<SourceVariable::VariableType const *> acsfuncArgTypes;
	std::vector<std::string> acsfuncArgNames;
	int acsfuncArgCount;
	SourceVariable::VariableType const * acsfuncReturn;
	make_expression_arglist(in, &acsfuncArgTypes, &acsfuncArgNames, &acsfuncArgCount, &acsfuncContext, &acsfuncReturn);

	SourceExpressionDS acsfuncExpression(make_expression_single(in, blocks, &acsfuncContext));
	acsfuncExpression.addLabel(acsfuncLabel);
	blocks->push_back(acsfuncExpression);
	blocks->push_back(make_expression_root_return(make_expression_root_data(acsfuncReturn, true, token.getPosition()), acsfuncContext, token.getPosition()));

	SourceVariable::VariableType const * acsfuncVarType(SourceVariable::get_VariableType_acsfunc(acsfuncReturn, acsfuncArgTypes));

	SourceVariable::VariableData_ACSFunc acsfuncVarData = {acsfuncVarType, acsfuncNumber};

	int acsfuncVarCount(acsfuncContext.getLimit(SourceVariable::SC_REGISTER));

	SourceVariable acsfuncVariable(acsfuncName, acsfuncVarData, token.getPosition());

	context->addVariable(acsfuncVariable);

	ObjectExpression::add_acsfunc(acsfuncLabel, acsfuncArgCount, acsfuncVarCount, acsfuncReturn->size());

	return make_expression_value_variable(acsfuncVariable, token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_single_asmfunc(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	std::string asmfuncName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	ObjectToken::ObjectCode asmfuncOCode(ObjectToken::get_code(in->get(SourceTokenC::TT_IDENTIFIER)));

	ObjectToken::ObjectCode asmfuncOCodeImmediate(ObjectToken::get_code(in->get(SourceTokenC::TT_IDENTIFIER)));

	std::vector<SourceVariable::VariableType const *> asmfuncArgs;
	SourceVariable::VariableType const * asmfuncReturn;
	make_expression_arglist(in, &asmfuncArgs, &asmfuncReturn);

	SourceVariable::VariableType const * asmfuncVarType(SourceVariable::get_VariableType_asmfunc(asmfuncReturn, asmfuncArgs));

	SourceVariable::VariableData_AsmFunc asmfuncVarData = {asmfuncVarType, asmfuncOCode, asmfuncOCodeImmediate};

	SourceVariable asmfuncVariable(asmfuncName, asmfuncVarData, token.getPosition());

	context->addVariable(asmfuncVariable);
	return make_expression_value_variable(asmfuncVariable, token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_single_const(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	SourceVariable::VariableType const * type(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));
	std::string name(in->get(SourceTokenC::TT_IDENTIFIER).getData());
	in->get(SourceTokenC::TT_OP_EQUALS);
	SourceExpressionDS data(make_expression(in, blocks, context));

	SourceVariable var(name, type, data.makeObject(), token.getPosition());

	context->addVariable(var);

	return make_expression_value_variable(var, token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_single_delay(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	return make_expression_root_delay(make_expression(in, blocks, context), token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_single_if(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
	SourceContext contextCondition(context, SourceContext::CT_BLOCK);
	SourceExpressionDS exprCondition(make_expression(in, blocks, &contextCondition));
	in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

	SourceContext contextIf(&contextCondition, SourceContext::CT_BLOCK);
	SourceExpressionDS exprIf(make_expression(in, blocks, &contextIf));
	SourceTokenC semicolonToken(in->get(SourceTokenC::TT_OP_SEMICOLON));

	if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER && in->peek().getData() == "else")
	{
		in->get();

		SourceContext contextElse(&contextCondition, SourceContext::CT_BLOCK);
		SourceExpressionDS exprElse(make_expression(in, blocks, &contextElse));
		in->unget(in->get(SourceTokenC::TT_OP_SEMICOLON));

		return make_expression_root_if(exprCondition, exprIf, exprElse, context, token.getPosition());
	}
	else
	{
		in->unget(semicolonToken);

		return make_expression_root_if(exprCondition, exprIf, context, token.getPosition());
	}
}
SourceExpressionDS SourceExpressionDS::make_expression_single_lnspec(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	std::string lnspecName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	ObjectExpression::int_t lnspecNumber(ObjectExpression::get_int(in->get(SourceTokenC::TT_INTEGER)));

	std::vector<SourceVariable::VariableType const *> lnspecArgs;
	SourceVariable::VariableType const * lnspecReturn;
	make_expression_arglist(in, &lnspecArgs, &lnspecReturn);

	SourceVariable::VariableType const * lnspecVarType(SourceVariable::get_VariableType_lnspec(lnspecReturn, lnspecArgs));

	SourceVariable::VariableData_LnSpec lnspecVarData = {lnspecVarType, lnspecNumber};

	SourceVariable lnspecVariable(lnspecName, lnspecVarData, token.getPosition());

	context->addVariable(lnspecVariable);
	return make_expression_value_variable(lnspecVariable, token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_single_native(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	std::string nativeName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	ObjectExpression::int_t nativeNumber(ObjectExpression::get_int(in->get(SourceTokenC::TT_INTEGER)));

	std::vector<SourceVariable::VariableType const *> nativeArgs;
	SourceVariable::VariableType const * nativeReturn;
	make_expression_arglist(in, &nativeArgs, &nativeReturn);

	SourceVariable::VariableType const * nativeVarType(SourceVariable::get_VariableType_native(nativeReturn, nativeArgs));

	SourceVariable::VariableData_Native nativeVarData = {nativeVarType, nativeNumber};

	SourceVariable nativeVariable(nativeName, nativeVarData, token.getPosition());

	context->addVariable(nativeVariable);
	return make_expression_value_variable(nativeVariable, token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_single_out(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	return make_expression_root_out(make_expression(in, blocks, context), token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_single_return(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	return make_expression_root_return(make_expression(in, blocks, context), *context, token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_single_script(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	SourceContext scriptContext(context, SourceContext::CT_SCRIPT);

	std::string scriptName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	SourceTokenC scriptNumberToken(in->get());
	ObjectExpression::int_t scriptNumber;
	if (scriptNumberToken.getType() == SourceTokenC::TT_INTEGER)
	{
		scriptNumber = ObjectExpression::get_int(scriptNumberToken);
	}
	else if (scriptNumberToken.getType() == SourceTokenC::TT_IDENTIFIER)
	{
		if (scriptNumberToken.getData() == "auto")
		{
			scriptNumber = ObjectExpression::get_script_number();
		}
		else
		{
			throw SourceException("expected 'auto'", token.getPosition(), "SourceExpressionDS");
		}
	}
	else
	{
		throw SourceException("expected TT_NUMBER of TT_IDENTIFIER", token.getPosition(), "SourceExpressionDS");
	}
	ObjectExpression::reserve_script_number(scriptNumber);

	// scriptName special cases
	if (scriptName == "auto")
	{
		std::ostringstream oss;
		oss << "script" << scriptNumber;
		scriptName = oss.str();
	}
	else if (scriptName == "void")
	{
		std::ostringstream oss;
		oss << "___script__" << scriptNumber << "__void___";
		scriptName = oss.str();
	}

	std::string scriptLabel(context->getLabel() + "script_" + scriptName);

	SourceTokenC scriptTypeToken(in->get(SourceTokenC::TT_IDENTIFIER));
	ObjectExpression::ScriptType scriptType(ObjectExpression::get_ScriptType(scriptTypeToken.getData(), scriptTypeToken.getPosition()));

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

	std::vector<SourceVariable::VariableType const *> scriptArgTypes;
	std::vector<std::string> scriptArgNames;
	int scriptArgCount;
	SourceVariable::VariableType const * scriptReturn;
	make_expression_arglist(in, &scriptArgTypes, &scriptArgNames, &scriptArgCount, &scriptContext, &scriptReturn);

	SourceVariable::VariableType const * scriptVarType(SourceVariable::get_VariableType_script(scriptReturn, scriptArgTypes));

	SourceVariable::VariableData_Script scriptVarData = {scriptVarType, scriptNumber};

	SourceExpressionDS scriptExpression(make_expression_single(in, blocks, &scriptContext));
	scriptExpression.addLabel(scriptLabel);
	blocks->push_back(scriptExpression);
	blocks->push_back(make_expression_root_return(make_expression_root_data(scriptReturn, true, token.getPosition()), scriptContext, token.getPosition()));

	int scriptVarCount(scriptContext.getLimit(SourceVariable::SC_REGISTER));

	SourceVariable scriptVariable(scriptName, scriptVarData, token.getPosition());

	context->addVariable(scriptVariable);

	ObjectExpression::add_script(scriptLabel, scriptNumber, scriptType, scriptArgCount, scriptVarCount, scriptFlags);

	return make_expression_value_variable(scriptVariable, token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_single_struct(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	std::string structName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	in->get(SourceTokenC::TT_OP_BRACE_O);

	std::vector<std::string> structNames;
	std::vector<SourceVariable::VariableType const *> structTypes;
	while (true)
	{
		if (in->peek().getType() == SourceTokenC::TT_OP_BRACE_C)
			break;

		structTypes.push_back(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));
		structNames.push_back(in->get(SourceTokenC::TT_IDENTIFIER).getData());

		in->get(SourceTokenC::TT_OP_SEMICOLON);
	}

	in->get(SourceTokenC::TT_OP_BRACE_C);

	SourceVariable::add_struct(structName, structNames, structTypes);

	return NULL;
}
SourceExpressionDS SourceExpressionDS::make_expression_single_term(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	return make_expression_root_term(token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_single_typedef(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	SourceTokenC typeToken(in->get(SourceTokenC::TT_IDENTIFIER));

	if (typeToken.getData() == "array")
	{
		SourceVariable::VariableType const * refType(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));
		in->get(SourceTokenC::TT_OP_BRACKET_O);
		int count((int)make_expression(in, blocks, context).makeObject()->resolveInt());
		in->get(SourceTokenC::TT_OP_BRACKET_C);

		SourceVariable::add_typedef(in->get(SourceTokenC::TT_IDENTIFIER).getData(), SourceVariable::get_VariableType_array(refType, count));
	}
	else if (typeToken.getData() == "script")
	{
		std::vector<SourceVariable::VariableType const *> scriptArgTypes;
		SourceVariable::VariableType const * scriptReturn;
		make_expression_arglist(in, &scriptArgTypes, &scriptReturn);

		SourceVariable::add_typedef(in->get(SourceTokenC::TT_IDENTIFIER).getData(), SourceVariable::get_VariableType_script(scriptReturn, scriptArgTypes));
	}
	else
	{
		SourceVariable::add_typedef(in->get(SourceTokenC::TT_IDENTIFIER).getData(), SourceVariable::get_VariableType(typeToken));
	}

	return NULL;
}
SourceExpressionDS SourceExpressionDS::make_expression_single_var(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	SourceVariable::StorageClass sc(SourceVariable::get_StorageClass(in->get(SourceTokenC::TT_IDENTIFIER)));
	SourceVariable::VariableType const * type(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));
	std::string name(in->get(SourceTokenC::TT_IDENTIFIER).getData());
	int addr(context->getCount(sc));

	SourceVariable var(context->getLabel() + name, name, addr, sc, type, token.getPosition());

	context->addVariable(var);

	return make_expression_value_variable(var, token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_single_void(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	return make_expression_root_void(make_expression(in, blocks, context), token.getPosition());
}
SourceExpressionDS SourceExpressionDS::make_expression_single_while(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context)
{
	in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
	SourceContext contextCondition(context, SourceContext::CT_BLOCK);
	SourceExpressionDS exprCondition(make_expression(in, blocks, &contextCondition));
	in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

	SourceContext contextWhile(&contextCondition, SourceContext::CT_BLOCK);
	SourceExpressionDS exprWhile(make_expression(in, blocks, &contextWhile));
	in->unget(in->get(SourceTokenC::TT_OP_SEMICOLON));

	return make_expression_root_while(exprCondition, exprWhile, context, token.getPosition());
}



