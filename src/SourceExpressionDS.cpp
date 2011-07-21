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

#include "ObjectExpression.hpp"
#include "ObjectToken.hpp"
#include "SourceContext.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"
#include "SourceTokenizerDS.hpp"

#include "SourceExpressionDS/Base.hpp"

#include <sstream>
#include <stdint.h>



SourceExpressionDS const SourceExpressionDS::nop;



SourceExpressionDS::SourceExpressionDS() : _expr(NULL)
{

}
SourceExpressionDS::SourceExpressionDS(SourceExpressionDS const & expr) : _expr(expr._expr ? expr._expr->clone() : NULL)
{

}
SourceExpressionDS::SourceExpressionDS(SourceExpressionDS_Base * const expr) : _expr(expr)
{

}
SourceExpressionDS::~SourceExpressionDS()
{
	delete _expr;
}

void SourceExpressionDS::addLabel(std::string const & label)
{
	if (_expr) _expr->addLabel(label);
}

SourceVariable::VariableType const * SourceExpressionDS::get_promoted_type(SourceVariable::VariableType const * const type1, SourceVariable::VariableType const * const type2)
{
	if (type1 == type2) return type1;

	if (type1->type == SourceVariable::VT_VOID) return type1;
	if (type2->type == SourceVariable::VT_VOID) return type2;

	if (type1->type == SourceVariable::VT_REAL) return type1;
	if (type2->type == SourceVariable::VT_REAL) return type2;

	if (type1->type == SourceVariable::VT_INT) return type1;
	if (type2->type == SourceVariable::VT_INT) return type2;

	if (type1->type == SourceVariable::VT_CHAR) return type1;
	if (type2->type == SourceVariable::VT_CHAR) return type2;

	if (type1->type == SourceVariable::VT_STRING) return type1;
	if (type2->type == SourceVariable::VT_STRING) return type2;

	return SourceVariable::get_VariableType(SourceVariable::VT_VOID);
}

SourcePosition const & SourceExpressionDS::getPosition() const
{
	return _expr ? _expr->getPosition() : SourcePosition::none;
}

SourceVariable::VariableType const * SourceExpressionDS::getType() const
{
	return _expr ? _expr->getType() : SourceVariable::get_VariableType(SourceVariable::VT_VOID);
}

std::vector<SourceExpressionDS> SourceExpressionDS::getVector() const
{
	return _expr ? _expr->getVector() : std::vector<SourceExpressionDS>();
}

bool SourceExpressionDS::isConstant() const
{
	return _expr ? _expr->isConstant() : true;
}

SourceExpressionDS SourceExpressionDS::make_expression(SourceTokenizerDS * const in, std::vector<SourceExpressionDS> * const blocks, SourceContext * const context)
{
	SourceExpressionDS expr(make_expression_single(in, blocks, context));

	while (true)
	{
		SourceTokenC token(in->get());

		switch (token.getType())
		{
		case SourceTokenC::TT_OP_ASTERISK:
			expr = make_expression_binary_mul(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_COMMA:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_EQUALS:
			return make_expression_binary_assign(expr, make_expression(in, blocks, context), token.getPosition());

		case SourceTokenC::TT_OP_MINUS:
			expr = make_expression_binary_sub(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_PARENTHESIS_C:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_PARENTHESIS_O:
		{
			std::vector<SourceExpressionDS> args;

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

			expr = make_expression_root_call(expr, args, token.getPosition());
		}
			break;

		case SourceTokenC::TT_OP_PERCENT:
			expr = make_expression_binary_mod(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_PERIOD:
			expr = make_expression_value_member(expr, in->get(SourceTokenC::TT_IDENTIFIER));
			break;

		case SourceTokenC::TT_OP_PLUS:
			expr = make_expression_binary_add(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_SEMICOLON:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_SLASH:
			expr = make_expression_binary_div(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		default:
			in->unget(token);
			throw SourceException("unexpected token type", token.getPosition(), "SourceExpressionDS");
		}
	}
}

SourceExpressionDS SourceExpressionDS::make_expression_cast(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position)
{
	switch (type->type)
	{
	case SourceVariable::VT_ASMFUNC: return make_expression_cast_void  (expr,       position);
	case SourceVariable::VT_CHAR:    return make_expression_cast_char  (expr,       position);
	case SourceVariable::VT_INT:     return make_expression_cast_int   (expr,       position);
	case SourceVariable::VT_LNSPEC:  return make_expression_cast_lnspec(expr, type, position);
	case SourceVariable::VT_NATIVE:  return make_expression_cast_native(expr, type, position);
	case SourceVariable::VT_REAL:    return make_expression_cast_real  (expr,       position);
	case SourceVariable::VT_SCRIPT:  return make_expression_cast_script(expr, type, position);
	case SourceVariable::VT_STRING:  return make_expression_cast_string(expr,       position);
	case SourceVariable::VT_STRUCT:  return make_expression_cast_struct(expr, type, position);
	case SourceVariable::VT_VOID:    return make_expression_cast_void  (expr,       position);
	}

	throw SourceException("attempt to cast to unknown", position, "SourceExpressionDS");
}

SourceExpressionDS SourceExpressionDS::make_expression_single(SourceTokenizerDS * const in, std::vector<SourceExpressionDS> * const blocks, SourceContext * const context)
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
		if (token.getData() == "asmfunc")
		{
			// asmfuncName
			std::string asmfuncName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

			// asmfuncOCode
			ObjectToken::ObjectCode asmfuncOCode(ObjectToken::get_code(in->get(SourceTokenC::TT_IDENTIFIER)));

			// asmfuncOCodeImmediate
			ObjectToken::ObjectCode asmfuncOCodeImmediate(ObjectToken::get_code(in->get(SourceTokenC::TT_IDENTIFIER)));

			// asmfuncArgs
			in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
			std::vector<SourceVariable::VariableType const *> asmfuncArgs;
			if (in->peek().getType() != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
			{
				asmfuncArgs.push_back(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));

				if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER)
					in->get(SourceTokenC::TT_IDENTIFIER);

				if (in->peek().getType() != SourceTokenC::TT_OP_COMMA)
					break;

				in->get(SourceTokenC::TT_OP_COMMA);
			}
			in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

			// asmfuncReturn
			in->get(SourceTokenC::TT_OP_MINUS_GT);
			SourceVariable::VariableType const * asmfuncReturn(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));

			// asmfuncVarType
			SourceVariable::VariableType const * asmfuncVarType(SourceVariable::get_VariableType_asmfunc(asmfuncReturn, asmfuncArgs));

			// asmfuncVarData
			SourceVariable::VariableData_AsmFunc asmfuncVarData = {asmfuncVarType, asmfuncOCode, asmfuncOCodeImmediate};

			// asmfuncVariable
			SourceVariable asmfuncVariable(asmfuncName, asmfuncVarData, token.getPosition());

			context->addVariable(asmfuncVariable);
			return make_expression_value_variable(asmfuncVariable, token.getPosition());
		}

		if (token.getData() == "delay")
			return make_expression_root_delay(make_expression(in, blocks, context), token.getPosition());

		if (token.getData() == "lnspec")
		{
			// lnspecName
			std::string lnspecName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

			// lnspecNumber
			ObjectExpression::int_t lnspecNumber(ObjectExpression::get_int(in->get(SourceTokenC::TT_INTEGER)));

			// lnspecArgs
			in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
			std::vector<SourceVariable::VariableType const *> lnspecArgs;
			if (in->peek().getType() != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
			{
				lnspecArgs.push_back(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));

				if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER)
					in->get(SourceTokenC::TT_IDENTIFIER);

				if (in->peek().getType() != SourceTokenC::TT_OP_COMMA)
					break;

				in->get(SourceTokenC::TT_OP_COMMA);
			}
			in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

			// lnspecReturn
			in->get(SourceTokenC::TT_OP_MINUS_GT);
			SourceVariable::VariableType const * lnspecReturn(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));

			// lnspecVarType
			SourceVariable::VariableType const * lnspecVarType(SourceVariable::get_VariableType_lnspec(lnspecReturn, lnspecArgs));

			// lnspecVarData
			SourceVariable::VariableData_LnSpec lnspecVarData = {lnspecVarType, lnspecNumber};

			// lnspecVariable
			SourceVariable lnspecVariable(lnspecName, lnspecVarData, token.getPosition());

			context->addVariable(lnspecVariable);
			return make_expression_value_variable(lnspecVariable, token.getPosition());
		}

		if (token.getData() == "native")
		{
			// nativeName
			std::string nativeName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

			// nativeNumber
			ObjectExpression::int_t nativeNumber(ObjectExpression::get_int(in->get(SourceTokenC::TT_INTEGER)));

			// nativeArgs
			in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
			std::vector<SourceVariable::VariableType const *> nativeArgs;
			if (in->peek().getType() != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
			{
				nativeArgs.push_back(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));

				if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER)
					in->get(SourceTokenC::TT_IDENTIFIER);

				if (in->peek().getType() != SourceTokenC::TT_OP_COMMA)
					break;

				in->get(SourceTokenC::TT_OP_COMMA);
			}
			in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

			// nativeReturn
			in->get(SourceTokenC::TT_OP_MINUS_GT);
			SourceVariable::VariableType const * nativeReturn(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));

			// nativeVarType
			SourceVariable::VariableType const * nativeVarType(SourceVariable::get_VariableType_native(nativeReturn, nativeArgs));

			// nativeVarData
			SourceVariable::VariableData_Native nativeVarData = {nativeVarType, nativeNumber};

			// nativeVariable
			SourceVariable nativeVariable(nativeName, nativeVarData, token.getPosition());

			context->addVariable(nativeVariable);
			return make_expression_value_variable(nativeVariable, token.getPosition());
		}

		if (token.getData() == "out")
			return make_expression_root_out(make_expression(in, blocks, context), token.getPosition());

		if (token.getData() == "return")
		{
			SourceExpressionDS expr(make_expression(in, blocks, context));

			if (expr.getType() != context->getReturnType())
				expr = make_expression_cast(expr, context->getReturnType(), token.getPosition());

			return make_expression_root_return(expr, token.getPosition());
		}

		if (token.getData() == "script")
		{
			// scriptContext
			SourceContext scriptContext(context, false);

			// scriptName
			std::string scriptName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

			// scriptNumber
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

			// scriptLabel
			std::string scriptLabel("script_" + scriptName);

			// scriptType
			SourceTokenC scriptTypeToken(in->get(SourceTokenC::TT_IDENTIFIER));
			ObjectExpression::ScriptType scriptType(ObjectExpression::get_ScriptType(scriptTypeToken.getData(), scriptTypeToken.getPosition()));

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

			// scriptArgs
			in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
			std::vector<SourceVariable::VariableType const *> scriptArgTypes;
			if (in->peek().getType() != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
			{
				SourceVariable::StorageClass sc(SourceVariable::SC_REGISTER);
				SourceVariable::VariableType const * type(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));
				std::string name(in->get(SourceTokenC::TT_IDENTIFIER).getData());
				int addr(scriptContext.getCount(sc));

				scriptArgTypes.push_back(type);
				scriptContext.addVariable(SourceVariable(name, name, addr, sc, type, token.getPosition()));

				if (in->peek().getType() != SourceTokenC::TT_OP_COMMA)
					break;

				in->get(SourceTokenC::TT_OP_COMMA);
			}
			int scriptArgs(scriptContext.getLimit(SourceVariable::SC_REGISTER));
			in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

			// scriptReturn
			in->get(SourceTokenC::TT_OP_MINUS_GT);
			SourceTokenC scriptReturnToken(in->get(SourceTokenC::TT_IDENTIFIER));
			SourceVariable::VariableType const * scriptReturn(SourceVariable::get_VariableType(scriptReturnToken));
			scriptContext.setReturnType(scriptReturn);

			// scriptVarType
			SourceVariable::VariableType const * scriptVarType(SourceVariable::get_VariableType_script(scriptReturn, scriptArgTypes));

			// scriptVarData
			SourceVariable::VariableData_Script scriptVarData = {scriptVarType, scriptNumber};

			// scriptExpression
			SourceExpressionDS scriptExpression(make_expression_single(in, blocks, &scriptContext));
			scriptExpression.addLabel(scriptLabel);
			blocks->push_back(scriptExpression);

			// scriptVars
			int scriptVars(scriptContext.getLimit(SourceVariable::SC_REGISTER));

			SourceVariable scriptVariable(scriptName, scriptVarData, token.getPosition());

			context->addVariable(scriptVariable);

			ObjectExpression::add_script(scriptLabel, scriptNumber, scriptType, scriptArgs, scriptVars, scriptFlags);

			return make_expression_value_variable(scriptVariable, token.getPosition());
		}

		if (token.getData() == "struct")
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

		if (token.getData() == "term")
			return make_expression_root_term(token.getPosition());

		if (token.getData() == "typedef")
		{
			SourceTokenC typeToken(in->get(SourceTokenC::TT_IDENTIFIER));

			if (typeToken.getData() == "script")
			{
				in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
				std::vector<SourceVariable::VariableType const *> scriptArgTypes;
				if (in->peek().getType() != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
				{
					scriptArgTypes.push_back(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));

					if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER)
						in->get(SourceTokenC::TT_IDENTIFIER);

					if (in->peek().getType() != SourceTokenC::TT_OP_COMMA)
						break;

					in->get(SourceTokenC::TT_OP_COMMA);
				}
				in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

				in->get(SourceTokenC::TT_OP_MINUS_GT);
				SourceVariable::VariableType const * scriptReturn(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));

				SourceVariable::add_typedef(in->get(SourceTokenC::TT_IDENTIFIER).getData(), SourceVariable::get_VariableType_script(scriptReturn, scriptArgTypes));
			}
			else
			{
				SourceVariable::add_typedef(in->get(SourceTokenC::TT_IDENTIFIER).getData(), SourceVariable::get_VariableType(typeToken));
			}

			return NULL;
		}

		if (token.getData() == "var")
		{
			SourceVariable::StorageClass sc(SourceVariable::get_StorageClass(in->get(SourceTokenC::TT_IDENTIFIER)));
			SourceVariable::VariableType const * type(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));
			std::string name(in->get(SourceTokenC::TT_IDENTIFIER).getData());
			int addr(context->getCount(sc));

			SourceVariable var(name, name, addr, sc, type, token.getPosition());

			context->addVariable(var);

			return make_expression_value_variable(var, token.getPosition());
		}

		if (token.getData() == "void")
			return make_expression_root_void(make_expression(in, blocks, context), token.getPosition());

		{
			SourceVariable::VariableType const * type(SourceVariable::get_VariableType_null(token.getData()));

			if (type)
				return make_expression_cast(make_expression_single(in, blocks, context), type, token.getPosition());
		}

		return make_expression_value_variable(context->getVariable(token), token.getPosition());

	case SourceTokenC::TT_INTEGER:
		return make_expression_value_int(token);

	case SourceTokenC::TT_STRING:
		return make_expression_value_string(token);

	case SourceTokenC::TT_OP_BRACE_O:
	{
		in->unget(token);
		std::vector<SourceExpressionDS> expressions;
		SourceContext blockContext(context, true);
		make_expressions(in, &expressions, blocks, &blockContext);
		return make_expression_root_block(expressions, token.getPosition());
	}

	case SourceTokenC::TT_OP_PARENTHESIS_O:
		expr = make_expression(in, blocks, context);
		in->get(SourceTokenC::TT_OP_PARENTHESIS_C);
		return expr;

	default:
		in->unget(token);
		throw SourceException("unexpected token type (single)", token.getPosition(), "SourceExpressionDS");
	}
}

SourceExpressionDS SourceExpressionDS::make_expressions(SourceTokenizerDS * const in)
{
	SourcePosition position(in->peek().getPosition());
	std::vector<SourceExpressionDS> expressions;
	std::vector<SourceExpressionDS> blocks;
	SourceContext context(&SourceContext::global_context, true);

	make_expressions(in, &expressions, &blocks, &context);

	for (size_t i(0); i < blocks.size(); ++i)
		expressions.push_back(blocks[i]);

	return make_expression_root_block(expressions, position);
}
void SourceExpressionDS::make_expressions(SourceTokenizerDS * const in, std::vector<SourceExpressionDS> * const expressions, std::vector<SourceExpressionDS> * const blocks, SourceContext * const context)
{
	in->get(SourceTokenC::TT_OP_BRACE_O);

	while (true)
	{
		if (in->peek().getType() == SourceTokenC::TT_OP_BRACE_C)
			break;

		expressions->push_back(make_expression(in, blocks, context));
		in->get(SourceTokenC::TT_OP_SEMICOLON);
	}

	in->get(SourceTokenC::TT_OP_BRACE_C);
}

void SourceExpressionDS::make_objects(std::vector<SourceExpressionDS> const & expressions, std::vector<ObjectToken> * const objects)
{
	for (uintptr_t index(0); index < expressions.size(); ++index)
		expressions[index].makeObjectsGet(objects);
}

void SourceExpressionDS::make_objects_call_asmfunc(std::vector<ObjectToken> * objects, SourceVariable::VariableData_AsmFunc const & data, std::vector<SourceExpressionDS> const & args, SourcePosition const & position)
{
	if (args.size() != data.type->types.size())
		throw SourceException("incorrect arg count to call asmfunc", position, "SourceExpressionDS");

	bool immediate(data.ocode_imm != ObjectToken::OCODE_NONE);

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i].getType() != data.type->types[i])
			throw SourceException("incorrect arg type to call asmfunc", args[i].getPosition(), "SourceExpressionDS");

		immediate = immediate && args[i].isConstant();
	}

	if (immediate)
	{
		std::vector<ObjectExpression> oargs;

		for (size_t i(0); i < args.size(); ++i)
			oargs.push_back(args[i].makeObject());

		objects->push_back(ObjectToken(data.ocode_imm, position, oargs));
	}
	else
	{
		if (data.ocode == ObjectToken::OCODE_NONE)
			throw SourceException("no ocode", position, "SourceExpressionDS");

		for (size_t i(0); i < args.size(); ++i)
			args[i].makeObjectsGet(objects);

		objects->push_back(ObjectToken(data.ocode, position));
	}
}

void SourceExpressionDS::make_objects_call_lnspec(std::vector<ObjectToken> * objects, SourceVariable::VariableData_LnSpec const & data, std::vector<SourceExpressionDS> const & args, SourcePosition const & position)
{
	if (args.size() != data.type->types.size())
		throw SourceException("incorrect arg count to call lnspec", position, "SourceExpressionDS");

	if (args.size() > 5)
		throw SourceException("too many args to call lnspec", position, "SourceExpressionDS");

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i].getType() != data.type->types[i])
			throw SourceException("incorrect arg type to call lnpsec", args[i].getPosition(), "SourceExpressionDS");

		args[i].makeObjectsGet(objects);
	}

	ObjectToken::ObjectCode ocode;

	ObjectExpression oarg0(ObjectExpression::create_value_int(0, position));
	ObjectExpression ospec(ObjectExpression::create_value_int(data.number, position));

	ObjectToken otok0(ObjectToken::OCODE_PUSHNUMBER, position, oarg0);

	if (data.type->callType->type == SourceVariable::VT_VOID)
	{
		switch (args.size())
		{
		case 0: ocode = ObjectToken::OCODE_LSPEC1; objects->push_back(otok0); break;
		case 1: ocode = ObjectToken::OCODE_LSPEC1; break;
		case 2: ocode = ObjectToken::OCODE_LSPEC2; break;
		case 3: ocode = ObjectToken::OCODE_LSPEC3; break;
		case 4: ocode = ObjectToken::OCODE_LSPEC4; break;
		case 5: ocode = ObjectToken::OCODE_LSPEC5; break;
		default: throw SourceException("unexpected arg count to call lnspec", position, "SourceExpressionDS");
		}
	}
	else
	{
		ocode = ObjectToken::OCODE_LSPEC5RESULT;

		for (size_t i(args.size()); i < 5; ++i)
			objects->push_back(otok0);
	}

	objects->push_back(ObjectToken(ocode, position, ospec));
}

void SourceExpressionDS::make_objects_call_native(std::vector<ObjectToken> * objects, SourceVariable::VariableData_Native const & data, std::vector<SourceExpressionDS> const & args, SourcePosition const & position)
{
	if (args.size() != data.type->types.size())
		throw SourceException("incorrect arg count to call native", position, "SourceExpressionDS");

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i].getType() != data.type->types[i])
			throw SourceException("incorrect arg type to call native", args[i].getPosition(), "SourceExpressionDS");

		args[i].makeObjectsGet(objects);
	}

	ObjectToken::ObjectCode ocode(ObjectToken::OCODE_CALLFUNC);
	ObjectExpression oargc(ObjectExpression::create_value_int(args.size(), position));
	ObjectExpression ofunc(ObjectExpression::create_value_int(data.number, position));

	std::vector<ObjectExpression> oargs;
	oargs.push_back(oargc);
	oargs.push_back(ofunc);

	objects->push_back(ObjectToken(ocode, position, oargs));
}

void SourceExpressionDS::make_objects_call_script(std::vector<ObjectToken> * const objects, SourceVariable::VariableType const * type, std::vector<SourceExpressionDS> const & args, SourcePosition const & position)
{
	if (args.size() != type->types.size())
		throw SourceException("incorrect arg count to call script", position, "SourceExpressionDS");

	if (args.size() > 3)
		throw SourceException("too many args to call script", position, "SourceExpressionDS");

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i].getType() != type->types[i])
			throw SourceException("incorrect arg type to call script", args[i].getPosition(), "SourceExpressionDS");

		args[i].makeObjectsGet(objects);
	}

	ObjectToken::ObjectCode code;
	ObjectExpression oarg0(ObjectExpression::create_value_int(0, position));
	ObjectExpression ospec(ObjectExpression::create_value_int(84, position));

	ObjectToken otok0(ObjectToken::OCODE_PUSHNUMBER, position, oarg0);

	if (type->callType->type == SourceVariable::VT_VOID)
	{
		switch (args.size())
		{
		case 0: code = ObjectToken::OCODE_LSPEC1; break;
		case 1: code = ObjectToken::OCODE_LSPEC2; break;
		case 2: code = ObjectToken::OCODE_LSPEC3; break;
		case 3: code = ObjectToken::OCODE_LSPEC4; break;
		default:
			throw SourceException("unexpected arg count to call script", position, "SourceExpressionDS");
		}
	}
	else
	{
		code = ObjectToken::OCODE_LSPEC5RESULT;

		for (size_t i(args.size()); i < 4; ++i)
			objects->push_back(otok0);
	}

	objects->push_back(ObjectToken(code, position, ospec));
}

ObjectExpression SourceExpressionDS::makeObject() const
{
	if (_expr)
		return _expr->makeObject();
	else
		throw SourceException("attempted to create object from NULL expression", SourcePosition::none, "SourceExpressionDS");
}
void SourceExpressionDS::makeObjectsCall(std::vector<ObjectToken> * const objects, std::vector<SourceExpressionDS> const & args) const
{
	if (_expr) _expr->makeObjectsCall(objects, args);
}
void SourceExpressionDS::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	if (_expr) _expr->makeObjectsGet(objects);
}
void SourceExpressionDS::makeObjectsGet(std::vector<ObjectToken> * const objects, std::vector<std::string> * const names) const
{
	if (_expr) _expr->makeObjectsGet(objects, names);
}
void SourceExpressionDS::makeObjectsSet(std::vector<ObjectToken> * const objects) const
{
	if (_expr) _expr->makeObjectsSet(objects);
}
void SourceExpressionDS::makeObjectsSet(std::vector<ObjectToken> * const objects, std::vector<std::string> * const names) const
{
	if (_expr) _expr->makeObjectsSet(objects, names);
}

SourceExpressionDS & SourceExpressionDS::operator = (SourceExpressionDS const & expr)
{
	delete _expr;
	_expr = expr._expr ? expr._expr->clone() : NULL;
	return *this;
}



void print_debug(std::ostream * const out, SourceExpressionDS const & in)
{
	*out << "SourceExpressionDS(";

	if (in._expr)
		in._expr->printDebug(out);
	else
		*out << "NULL";

	*out << ")";
}



