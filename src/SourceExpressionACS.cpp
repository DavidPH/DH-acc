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

/* SourceExpressionACS.cpp
**
** SourceExpressionACS methods.
*/

#include "SourceExpressionACS.hpp"

#include "SourceBlockC.hpp"
#include "SourceContext.hpp"
#include "SourceTokenC.hpp"

#include <stdint.h>



SourceExpressionACS const SourceExpressionACS::nop; // TODO: SourceExpressionACSNop.



SourceExpressionACS::SourceExpressionACS() : _expr(NULL)
{

}
SourceExpressionACS::SourceExpressionACS(SourceExpressionACS const & expr) : _expr(expr._expr->clone())
{

}
SourceExpressionACS::SourceExpressionACS(SourceExpressionACSBase * const expr) : _expr(expr)
{

}
SourceExpressionACS::~SourceExpressionACS()
{
	delete _expr;
}

SourcePosition const & SourceExpressionACS::getPosition() const
{
	return _expr->getPosition();
}

SourceExpressionACSBase::ExpressionType SourceExpressionACS::getType() const
{
	return _expr->getType();
}

bool SourceExpressionACS::isConstant() const
{
	return _expr->isConstant();
}

SourceExpressionACS SourceExpressionACS::make_expression(std::vector<SourceTokenC> const & tokens, SourceContext const & context)
{
	for (uintptr_t index(0); index < tokens.size(); ++index)
	{
		if (tokens[index].getType() == SourceTokenC::TT_OP_PLUS)
		{
			std::vector<SourceTokenC> tokensL(&tokens.front(), &tokens[index]);
			std::vector<SourceTokenC> tokensR(&tokens[index+1], &*tokens.end());

			SourceExpressionACS exprL(make_expression(tokensL, context));
			SourceExpressionACS exprR(make_expression(tokensR, context));

			return make_expression_binary_add(exprL, exprR);
		}
	}

	return SourceExpressionACS::nop;
}

void SourceExpressionACS::make_expressions(SourceBlockC const & block, std::vector<SourceExpressionACS> * const expressions)
{
	SourceContext context;

	SourceTokenC const & token(block.getToken(0));

	if (token.getType() == SourceTokenC::TT_IDENTIFIER)
	{
		std::string const & id(token.getData());

		if (id == "auto")
		{

		}
		else if (id == "register")
		{

		}
		else if (id == "static")
		{

		}
	}

	// If we get here, it's a normal expression.

	expressions->push_back(make_expression(std::vector<SourceTokenC>(&block.getTokens().front(), &block.getTokens().back()), context));
}

void SourceExpressionACS::make_objects(std::vector<SourceExpressionACS> const & expressions, std::vector<ObjectToken> * const objects)
{
	for (uintptr_t index(0); index < expressions.size(); ++index)
		expressions[index].makeObjects(objects);
}

void SourceExpressionACS::makeObjects(std::vector<ObjectToken> * const objects) const
{
	_expr->makeObjects(objects);
}

SourceExpressionACS & SourceExpressionACS::operator = (SourceExpressionACS const & expr)
{
	delete _expr;
	_expr = expr._expr->clone();
	return *this;
}



void print_debug(std::ostream * const out, SourceExpressionACS const & expression)
{
	expression._expr->printDebug(out);
}



