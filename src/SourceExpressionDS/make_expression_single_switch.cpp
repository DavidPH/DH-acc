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

/* SourceExpressionDS/make_expression_single_switch.cpp
**
** Defines the SourceExpressionDS::make_expression_single_switch function.
*/

#include "../SourceExpressionDS.hpp"

#include "../SourceContext.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"



SourceExpression::Pointer SourceExpressionDS::make_expression_single_switch(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceContext contextSwitch(context, SourceContext::CT_BLOCK);
	SourceExpression::Pointer exprSwitch(make_expression_single(in, blocks, &contextSwitch));

	in->unget(in->get(SourceTokenC::TT_OP_BRACE_O));

	SourceContext contextCases(&contextSwitch, SourceContext::CT_SWITCH);
	SourceExpression::Pointer exprCases(make_expression(in, blocks, &contextCases));

	return create_branch_switch(exprSwitch, exprCases, &contextCases, token.getPosition());
}



