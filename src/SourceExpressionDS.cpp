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
#include "ObjectVector.hpp"
#include "SourceContext.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"
#include "SourceTokenizerDS.hpp"

#include "SourceExpressionDS/Base.hpp"

#include <sstream>
#include <stdint.h>



std::map<std::string, SourceExpressionDS::expression_single_handler> SourceExpressionDS::_expression_single_handlers;
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

void SourceExpressionDS::init()
{
	_expression_single_handlers["acsfunc"] = make_expression_single_acsfunc;
	_expression_single_handlers["asmfunc"] = make_expression_single_asmfunc;
	_expression_single_handlers["delay"]   = make_expression_single_delay;
	_expression_single_handlers["if"]      = make_expression_single_if;
	_expression_single_handlers["lnspec"]  = make_expression_single_lnspec;
	_expression_single_handlers["native"]  = make_expression_single_native;
	_expression_single_handlers["out"]     = make_expression_single_out;
	_expression_single_handlers["return"]  = make_expression_single_return;
	_expression_single_handlers["script"]  = make_expression_single_script;
	_expression_single_handlers["struct"]  = make_expression_single_struct;
	_expression_single_handlers["term"]    = make_expression_single_term;
	_expression_single_handlers["typedef"] = make_expression_single_typedef;
	_expression_single_handlers["var"]     = make_expression_single_var;
	_expression_single_handlers["void"]    = make_expression_single_void;
	_expression_single_handlers["while"]   = make_expression_single_while;
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

		case SourceTokenC::TT_OP_BRACKET_C:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_BRACKET_O:
			expr = make_expression_binary_array(expr, make_expression(in, blocks, context), token.getPosition());
			in->get(SourceTokenC::TT_OP_BRACKET_C);
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

SourceExpressionDS SourceExpressionDS::make_expression_cast(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position)
{
	switch (type->type)
	{
	case SourceVariable::VT_ACSFUNC: return make_expression_cast_acsfunc(expr, type, position);
	case SourceVariable::VT_ARRAY:   return make_expression_cast_array  (expr, type, position);
	case SourceVariable::VT_ASMFUNC: return make_expression_cast_void   (expr,       position);
	case SourceVariable::VT_CHAR:    return make_expression_cast_char   (expr,       position);
	case SourceVariable::VT_INT:     return make_expression_cast_int    (expr,       position);
	case SourceVariable::VT_LNSPEC:  return make_expression_cast_lnspec (expr, type, position);
	case SourceVariable::VT_NATIVE:  return make_expression_cast_native (expr, type, position);
	case SourceVariable::VT_REAL:    return make_expression_cast_real   (expr,       position);
	case SourceVariable::VT_SCRIPT:  return make_expression_cast_script (expr, type, position);
	case SourceVariable::VT_STRING:  return make_expression_cast_string (expr,       position);
	case SourceVariable::VT_STRUCT:  return make_expression_cast_struct (expr, type, position);
	case SourceVariable::VT_VOID:    return make_expression_cast_void   (expr,       position);
	}

	throw SourceException("attempt to cast to unknown", position, "SourceExpressionDS");
}

SourceExpressionDS SourceExpressionDS::make_expressions(SourceTokenizerDS * const in)
{
	SourcePosition position(in->peek().getPosition());
	std::vector<SourceExpressionDS> expressions;
	std::vector<SourceExpressionDS> blocks;
	SourceContext context(&SourceContext::global_context, SourceContext::CT_BLOCK);

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

void SourceExpressionDS::make_objects(std::vector<SourceExpressionDS> const & expressions, ObjectVector * objects)
{
	for (uintptr_t index(0); index < expressions.size(); ++index)
		expressions[index].makeObjectsGet(objects);
}

void SourceExpressionDS::make_objects_call_acsfunc(ObjectVector * objects, SourceVariable::VariableData_ACSFunc const & data, std::vector<SourceExpressionDS> const & args, SourcePosition const & position)
{
	if (args.size() != data.type->types.size())
		throw SourceException("incorrect arg count to call acsfunc", position, "SourceExpressionDS");

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i].getType() != data.type->types[i])
			throw SourceException("incorrect arg type to call acsfunc", args[i].getPosition(), "SourceExpressionDS");

		args[i].makeObjectsGet(objects);
	}

	objects->setPosition(position);

	ObjectToken::ObjectCode ocode;
	ObjectExpression ofunc(objects->getValue(data.number));

	if (data.type->callType->type == SourceVariable::VT_VOID)
		ocode = ObjectToken::OCODE_CALLZDACSDISCARD;
	else
		ocode = ObjectToken::OCODE_CALLZDACS;

	objects->addToken(ocode, ofunc);
}

void SourceExpressionDS::make_objects_call_asmfunc(ObjectVector * objects, SourceVariable::VariableData_AsmFunc const & data, std::vector<SourceExpressionDS> const & args, SourcePosition const & position)
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

		objects->setPosition(position).addToken(data.ocode_imm, oargs);
	}
	else
	{
		if (data.ocode == ObjectToken::OCODE_NONE)
			throw SourceException("no ocode", position, "SourceExpressionDS");

		for (size_t i(0); i < args.size(); ++i)
			args[i].makeObjectsGet(objects);

		objects->setPosition(position).addToken(data.ocode);
	}
}

void SourceExpressionDS::make_objects_call_lnspec(ObjectVector * objects, SourceVariable::VariableData_LnSpec const & data, std::vector<SourceExpressionDS> const & args, SourcePosition const & position)
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

	objects->setPosition(position);

	ObjectToken::ObjectCode ocode;
	ObjectExpression ospec(objects->getValue(data.number));

	if (data.type->callType->type == SourceVariable::VT_VOID)
	{
		switch (args.size())
		{
		case 0: ocode = ObjectToken::OCODE_LSPEC1; objects->addTokenPushZero(); break;
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
			objects->addTokenPushZero();
	}

	objects->addToken(ocode, ospec);
}

void SourceExpressionDS::make_objects_call_native(ObjectVector * objects, SourceVariable::VariableData_Native const & data, std::vector<SourceExpressionDS> const & args, SourcePosition const & position)
{
	if (args.size() != data.type->types.size())
		throw SourceException("incorrect arg count to call native", position, "SourceExpressionDS");

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i].getType() != data.type->types[i])
			throw SourceException("incorrect arg type to call native", args[i].getPosition(), "SourceExpressionDS");

		args[i].makeObjectsGet(objects);
	}

	objects->setPosition(position);

	ObjectToken::ObjectCode ocode(ObjectToken::OCODE_CALLZDFUNC);
	ObjectExpression oargc(objects->getValue((int)args.size()));
	ObjectExpression ofunc(objects->getValue(data.number));

	objects->addToken(ocode, oargc, ofunc);
}

void SourceExpressionDS::make_objects_call_script(ObjectVector * objects, SourceVariable::VariableType const * type, std::vector<SourceExpressionDS> const & args, SourcePosition const & position)
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

	objects->setPosition(position);

	ObjectToken::ObjectCode code;
	ObjectExpression ospec(objects->getValue(84));

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
			objects->addTokenPushZero();
	}

	objects->addToken(code, ospec);
}

ObjectExpression SourceExpressionDS::makeObject() const
{
	if (_expr)
		return _expr->makeObject();
	else
		throw SourceException("attempted to create object from NULL expression", SourcePosition::none, "SourceExpressionDS");
}

void SourceExpressionDS::makeObjectsCall(ObjectVector * objects, std::vector<SourceExpressionDS> const & args) const
{
	if (_expr) _expr->makeObjectsCall(objects, args);
}

void SourceExpressionDS::makeObjectsGet(ObjectVector * objects) const
{
	if (_expr) _expr->makeObjectsGet(objects);
}
void SourceExpressionDS::makeObjectsGetArray(ObjectVector * objects, int dimensions) const
{
	if (_expr) _expr->makeObjectsGetArray(objects, dimensions);
}
void SourceExpressionDS::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names) const
{
	if (_expr) _expr->makeObjectsGetMember(objects, names);
}

void SourceExpressionDS::makeObjectsSet(ObjectVector * objects) const
{
	if (_expr) _expr->makeObjectsSet(objects);
}
void SourceExpressionDS::makeObjectsSetArray(ObjectVector * objects, int dimensions) const
{
	if (_expr) _expr->makeObjectsSetArray(objects, dimensions);
}
void SourceExpressionDS::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names) const
{
	if (_expr) _expr->makeObjectsSetMember(objects, names);
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



