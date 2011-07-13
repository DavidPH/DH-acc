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
#include "SourceContext.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"
#include "SourceTokenizerDS.hpp"
#include "SourceVariable.hpp"

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

SourceExpressionDS::ExpressionType SourceExpressionDS::get_promoted_type(ExpressionType const type1, ExpressionType const type2)
{
	if (type1 == type2) return type1;

	if (type1 == ET_VOID   || type2 == ET_VOID)   return ET_VOID;
	if (type1 == ET_FIXED  || type2 == ET_FIXED)  return ET_FIXED;
	if (type1 == ET_INT    || type2 == ET_INT)    return ET_INT;
	if (type1 == ET_STRING || type2 == ET_STRING) return ET_STRING;

	return ET_INT;
}

SourcePosition const & SourceExpressionDS::getPosition() const
{
	return _expr ? _expr->getPosition() : SourcePosition::none;
}

SourceExpressionDS::ExpressionType SourceExpressionDS::getType() const
{
	return _expr ? _expr->getType() : ET_VOID;
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

		case SourceTokenC::TT_OP_MINUS:
			expr = make_expression_binary_sub(expr, make_expression_single(in, blocks, context), token.getPosition());
			break;

		case SourceTokenC::TT_OP_COMMA:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_EQUALS:
			return make_expression_binary_assign(expr, make_expression(in, blocks, context), token.getPosition());

		case SourceTokenC::TT_OP_PARENTHESIS_C:
			in->unget(token);
			return expr;

		case SourceTokenC::TT_OP_PERCENT:
			expr = make_expression_binary_mod(expr, make_expression_single(in, blocks, context), token.getPosition());
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

SourceExpressionDS SourceExpressionDS::make_expression_cast(SourceExpressionDS const & expr, ExpressionType const type, SourcePosition const & position)
{
	switch (type)
	{
	case ET_FIXED:  return make_expression_cast_fixed(expr, position);
	case ET_INT:    return make_expression_cast_int(expr, position);
	case ET_STRING: return make_expression_cast_string(expr, position);
	case ET_VOID: throw SourceException("attempt to cast to ET_VOID", position, "SourceExpressionDS");
	}

	throw SourceException("attempt to cast to unknown", position, "SourceExpressionDS");
}

SourceExpressionDS SourceExpressionDS::make_expression_single(SourceTokenizerDS * const in, std::vector<SourceExpressionDS> * const blocks, SourceContext * const context)
{
	SourceExpressionDS expr;
	SourceTokenC token(in->get());

	switch (token.getType())
	{
	case SourceTokenC::TT_IDENTIFIER:
		if (token.getData() == "lspec")
		{
			in->get(SourceTokenC::TT_OP_PARENTHESIS_O);

			SourceExpressionDS spec(make_expression(in, blocks, context));

			if (spec.getType() != ET_INT)
				spec = make_expression_cast_int(spec, token.getPosition());

			std::vector<SourceExpressionDS> args;

			while (true)
			{
				SourceTokenC token(in->get());

				if (token.getType() == SourceTokenC::TT_OP_PARENTHESIS_C)
					break;

				if (token.getType() != SourceTokenC::TT_OP_COMMA)
					throw SourceException("expected TT_OP_PARENTHESIS_C or TT_OP_COMMA", token.getPosition(), "SourceExpressionDS");

				args.push_back(make_expression(in, blocks, context));
			}

			return make_expression_root_lspec(spec, args, token.getPosition());
		}

		if (token.getData() == "out")
			return make_expression_root_out(make_expression(in, blocks, context), token.getPosition());

		if (token.getData() == "script")
		{
			SourceTokenC scriptNumberToken(in->get(SourceTokenC::TT_NUMBER));
			int32_t scriptNumber;
			{
				std::istringstream iss(scriptNumberToken.getData());
				iss >> scriptNumber;
			}

			SourceTokenC scriptTypeToken(in->get(SourceTokenC::TT_IDENTIFIER));
			ObjectExpression::ScriptType scriptType(ObjectExpression::get_ScriptType(scriptTypeToken.getData(), scriptTypeToken.getPosition()));

			int scriptFlags(0);
			while (true)
			{
				SourceTokenC token(in->get());

				if (token.getType() == SourceTokenC::TT_IDENTIFIER)
				{
					scriptFlags |= ObjectExpression::get_ScriptFlag(token.getData(), token.getPosition());
				}
				else if (token.getType() == SourceTokenC::TT_OP_PARENTHESIS_O)
				{
					in->unget(token);
					break;
				}
				else
				{
					throw SourceException("expected TT_IDENTIFIER or TT_OP_PARENTHESIS_O", token.getPosition(), "SourceExpressionDS");
				}
			}

			in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
			// TODO: scriptArgs
			in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

			SourceContext scriptContext;
			SourceExpressionDS scriptExpression(make_expression_single(in, blocks, &scriptContext));

			std::string scriptLabel;
			{
				std::ostringstream oss;
				oss << "script" << scriptNumber;
				scriptLabel = oss.str();
			}

			scriptExpression.addLabel(scriptLabel);
			blocks->push_back(scriptExpression);
			ObjectExpression::add_script(scriptLabel, scriptNumber, scriptType, 0, scriptFlags);

			return make_expression_value_int(scriptNumberToken);
		}

		if (token.getData() == "term")
		{
			in->unget(in->get(SourceTokenC::TT_OP_SEMICOLON));
			return make_expression_root_term(token.getPosition());
		}

		if (token.getData() == "var")
		{
			SourceVariable::StorageClass sc  (SourceVariable::get_StorageClass(in->get(SourceTokenC::TT_IDENTIFIER)));
			SourceVariable::VariableType type(SourceVariable::get_VariableType(in->get(SourceTokenC::TT_IDENTIFIER)));
			std::string name(in->get(SourceTokenC::TT_IDENTIFIER).getData());

			SourceVariable var(name, name, context->getAddress(sc), sc, type, token.getPosition());

			context->addVariable(var);

			return make_expression_value_variable(var, token.getPosition());
		}

		if (token.getData() == "void")
			return make_expression_root_void(make_expression(in, blocks, context), token.getPosition());

		return make_expression_value_variable(context->getVariable(token), token.getPosition());

	case SourceTokenC::TT_NUMBER:
		return make_expression_value_number(token);

	case SourceTokenC::TT_STRING:
		return make_expression_value_string(ObjectExpression::create_value_symbol(ObjectExpression::add_string(token.getData() + '\0'), token.getPosition()), token.getPosition());

	case SourceTokenC::TT_OP_BRACE_O:
	{
		in->unget(token);
		std::vector<SourceExpressionDS> expressions;
		SourceContext blockContext(*context);
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

SourceExpressionDS SourceExpressionDS::make_expression_value_number(SourceTokenC const & token)
{
	if (token.getData().find_first_of('.') == std::string::npos)
		return make_expression_value_int(token);
	else
		return make_expression_value_fixed(token);
}

SourceExpressionDS SourceExpressionDS::make_expressions(SourceTokenizerDS * const in)
{
	SourcePosition position(in->peek().getPosition());
	std::vector<SourceExpressionDS> expressions;
	std::vector<SourceExpressionDS> blocks;
	SourceContext context;

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

ObjectExpression SourceExpressionDS::makeObject() const
{
	if (_expr)
		return _expr->makeObject();
	else
		throw SourceException("attempted to create object from NULL expression", SourcePosition::none, "SourceExpressionDS");
}
void SourceExpressionDS::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	if (_expr) _expr->makeObjectsGet(objects);
}
void SourceExpressionDS::makeObjectsSet(std::vector<ObjectToken> * const objects) const
{
	if (_expr) _expr->makeObjectsSet(objects);
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
void print_debug(std::ostream * const out, SourceExpressionDS::ExpressionType const in)
{
	switch (in)
	{
	case SourceExpressionDS::ET_FIXED:  *out << "ET_FIXED";  break;
	case SourceExpressionDS::ET_INT:    *out << "ET_INT";    break;
	case SourceExpressionDS::ET_STRING: *out << "ET_STRING"; break;
	case SourceExpressionDS::ET_VOID:   *out << "ET_VOID";   break;
	}
}



