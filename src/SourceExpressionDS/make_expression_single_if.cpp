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

/* SourceExpressionDS/make_expression_single_if.cpp
**
** Defines the SourceExpressionDS::make_expression_single_if function.
*/

#include "../SourceExpressionDS.hpp"

#include "../SourceContext.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_expression_single_if
//
SRCEXPDS_EXPRSINGLE_DEFN(if)
{
	SourceContext::Reference contextCondition = SourceContext::create(context, SourceContext::CT_BLOCK);
	SourceExpression::Pointer exprCondition(make_expression_single(in, blocks, contextCondition));

	SourceContext::Reference contextIf = SourceContext::create(contextCondition, SourceContext::CT_BLOCK);
	SourceExpression::Pointer exprIf(make_expression_single(in, blocks, contextIf));

	if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER && in->peek().getData() == "else")
	{
		in->get();

		SourceContext::Reference contextElse = SourceContext::create(contextCondition, SourceContext::CT_BLOCK);
		SourceExpression::Pointer exprElse(make_expression_single(in, blocks, contextElse));

		return create_branch_if(exprCondition, exprIf, exprElse, context, token.getPosition());
	}
	else
	{
		return create_branch_if(exprCondition, exprIf, context, token.getPosition());
	}
}

// EOF

