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
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"



SourceExpression::Pointer SourceExpressionDS::make_expression_single_function(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	// functionContext
	SourceContext functionContext(context, SourceContext::CT_FUNCTION);

	// functionNumber
	int functionNumber(ObjectExpression::get_function_count());

	// functionName
	std::string functionName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	// functionLabel
	std::string functionLabel(context->getLabel() + "function_" + functionName);

	// functionArgTypes/Names/Count functionReturn
	std::vector<SourceVariable::VariableType const *> functionArgTypes;
	std::vector<std::string> functionArgNames;
	int functionArgCount;
	SourceVariable::VariableType const * functionReturn;
	make_expression_arglist(in, blocks, context, &functionArgTypes, &functionArgNames, &functionArgCount, &functionContext, &functionReturn);

	// functionExpression
	SourceExpression::Pointer functionExpression(make_expression_single(in, blocks, &functionContext));
	functionExpression->addLabel(functionLabel);
	blocks->push_back(functionExpression);
	blocks->push_back(create_branch_return(create_value_data(functionReturn, true, token.getPosition()), &functionContext, token.getPosition()));

	// functionVarType
	SourceVariable::VariableType const * functionVarType(SourceVariable::get_VariableType_function(functionReturn, functionArgTypes));

	// functionVarData
	SourceVariable::VariableData_Function functionVarData = {functionVarType, functionNumber};

	// functionVarCount
	int functionVarCount(functionContext.getLimit(SourceVariable::SC_REGISTER));

	// functionVariable
	SourceVariable functionVariable(functionName, functionVarData, token.getPosition());

	context->addVariable(functionVariable);

	ObjectExpression::add_function(functionLabel, functionArgCount, functionVarCount, functionReturn->size());

	return create_value_variable(functionVariable, token.getPosition());
}



