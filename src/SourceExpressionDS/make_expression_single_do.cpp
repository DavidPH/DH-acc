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

/* SourceExpressionDS/make_expression_single_do.cpp
**
** Defines the SourceExpressionDS::make_expression_single_do function.
*/

#include "../SourceExpressionDS.hpp"

#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenizerDS.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_do
//
SRCEXPDS_EXPRSINGLE_DEFN(do)
{
	SourceContext::Reference contextLoop = SourceContext::create(context, SourceContext::CT_LOOP);
	SourceExpression::Pointer exprLoop(make_expression_single(in, blocks, contextLoop));

	SourceTokenC tokenWhile(in->get(SourceTokenC::TT_IDENTIFIER));

	if (tokenWhile.getData() != "while")
		throw SourceException("expected 'while' got '" + tokenWhile.getData() + "'", token.getPosition(), "SourceExpressionDS");

	SourceContext::Reference contextCondition = SourceContext::create(contextLoop, SourceContext::CT_BLOCK);
	SourceExpression::Pointer exprCondition(make_expression_single(in, blocks, contextCondition));

	return create_branch_do(exprCondition, exprLoop, contextCondition, token.getPosition());
}

// EOF

