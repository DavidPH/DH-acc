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

/* SourceExpressionDS/make_expression_single_function.cpp
**
** Defines the SourceExpressionDS::make_expression_single_function function.
*/

#include "../SourceExpressionDS.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableType.hpp"



SourceExpression::Pointer SourceExpressionDS::make_expression_single_function(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	// functionContext
	SourceContext functionContext(context, SourceContext::CT_FUNCTION);

	// functionName
	std::string functionName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	// functionNameObject
	std::string functionNameObject(context->getLabel() + functionName);

	// functionLabel
	std::string functionLabel("function_" + functionNameObject);

	// functionArgTypes/Names/Count functionReturn
	std::vector<VariableType const *> functionArgTypes;
	std::vector<std::string> functionArgNames;
	int functionArgCount;
	VariableType const * functionReturn;
	make_expression_arglist(in, blocks, context, &functionArgTypes, &functionArgNames, &functionArgCount, &functionContext, &functionReturn);

	// functionExpression
	SourceExpression::Pointer functionExpression(make_expression_single(in, blocks, &functionContext));
	functionExpression->addLabel(functionLabel);
	blocks->push_back(functionExpression);
	blocks->push_back(create_branch_return(create_value_data(functionReturn, true, token.getPosition()), &functionContext, token.getPosition()));

	// functionVarType
	VariableType const * functionVarType(VariableType::get_function(functionReturn, functionArgTypes));

	// functionVarCount
	int functionVarCount(functionContext.getLimit(SourceVariable::SC_REGISTER));

	// functionVariable
	SourceVariable::Pointer functionVariable(SourceVariable::create_constant(functionName, functionVarType, functionNameObject, token.getPosition()));

	context->addVariable(functionVariable);
	ObjectExpression::add_function(functionNameObject, functionLabel, functionArgCount, functionVarCount, functionReturn->size(token.getPosition()));
	return create_value_variable(functionVariable, token.getPosition());
}


