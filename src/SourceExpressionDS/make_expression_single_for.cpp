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

/* SourceExpressionDS/make_expression_single_for.cpp
**
** Defines the SourceExpressionDS::make_expression_single_for function.
*/

#include "../SourceExpressionDS.hpp"

#include "../SourceContext.hpp"
#include "../SourceTokenizerDS.hpp"



SourceExpression::Pointer SourceExpressionDS::make_expression_single_for(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	in->get(SourceTokenC::TT_OP_PARENTHESIS_O);

	SourceContext contextInit(context, SourceContext::CT_BLOCK);
	SourceExpression::Pointer exprInit(make_expression(in, blocks, &contextInit));

	in->get(SourceTokenC::TT_OP_SEMICOLON);

	SourceContext contextCond(&contextInit, SourceContext::CT_BLOCK);
	SourceExpression::Pointer exprCond(make_expression(in, blocks, &contextCond));

	in->get(SourceTokenC::TT_OP_SEMICOLON);

	SourceContext contextIter(&contextCond, SourceContext::CT_BLOCK);
	SourceExpression::Pointer exprIter(make_expression(in, blocks, &contextIter));

	in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

	SourceContext contextLoop(&contextCond, SourceContext::CT_LOOP);
	SourceExpression::Pointer exprLoop(make_expression_single(in, blocks, &contextLoop));

	return create_branch_for(exprInit, exprCond, exprIter, exprLoop, &contextLoop, token.getPosition());
}



