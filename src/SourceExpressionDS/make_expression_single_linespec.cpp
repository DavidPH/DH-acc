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

/* SourceExpressionDS/make_expression_single_linespec.cpp
**
** Defines the SourceExpressionDS::make_expression_single_linespec function.
*/

#include "../SourceExpressionDS.hpp"

#include "../SourceContext.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"



SourceExpression::Pointer SourceExpressionDS::make_expression_single_linespec(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	// linespecName
	std::string linespecName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	// linespecArgTypes linespecReturn
	std::vector<SourceVariable::VariableType const *> linespecArgTypes;
	SourceVariable::VariableType const * linespecReturn;
	make_expression_arglist(in, blocks, context, &linespecArgTypes, &linespecReturn);

	// linespecNumber
	in->get(SourceTokenC::TT_OP_AT);
	ObjectExpression::int_t linespecNumber(ObjectExpression::get_int(in->get(SourceTokenC::TT_INTEGER)));

	// linespecVarType
	SourceVariable::VariableType const * linespecVarType(SourceVariable::get_VariableType_linespec(linespecReturn, linespecArgTypes));

	// linespecVarData
	SourceVariable::VariableData_LineSpec linespecVarData = {linespecVarType, linespecNumber};

	// linespecVariable
	SourceVariable linespecVariable(linespecName, linespecVarData, token.getPosition());

	context->addVariable(linespecVariable);
	return create_value_variable(linespecVariable, token.getPosition());
}


