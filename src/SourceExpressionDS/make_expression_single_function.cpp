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
#include "../ost_type.hpp"
#include "../SourceContext.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableType.hpp"



SRCEXPDS_EXPRSINGLE_DEFN(extern_function)
{
	// functionName
	std::string functionName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	// functionNameObject
	std::string functionNameObject(functionName);

	// functionLabel
	std::string functionLabel("function_" + functionNameObject);

	// functionArgTypes/Count functionReturn
	std::vector<VariableType const *> functionArgTypes;
	int functionArgCount;
	VariableType const * functionReturn;
	make_expression_arglist(in, blocks, context, &functionArgTypes, NULL, &functionArgCount, NULL, &functionReturn);

	// functionVarType
	VariableType const * functionVarType(VariableType::get_function(functionReturn, functionArgTypes));

	// functionVariable
	SourceVariable::Pointer functionVariable;
	if (target_type == TARGET_HexPP)
		functionVariable = SourceVariable::create_constant(functionName, functionVarType, functionLabel, token.getPosition());
	else
		functionVariable = SourceVariable::create_constant(functionName, functionVarType, functionNameObject, token.getPosition());
	context->addVariable(functionVariable);

	ObjectExpression::add_function(functionNameObject, functionLabel, functionArgCount, functionArgCount, functionReturn->size(token.getPosition()), "");
	return create_value_variable(functionVariable, token.getPosition());
}

SRCEXPDS_EXPRSINGLE_DEFN(function)
{
	// functionArgClass
	SourceVariable::StorageClass functionArgClass;

	if (target_type == TARGET_HexPP)
		functionArgClass = SourceVariable::SC_AUTO;
	else
		functionArgClass = SourceVariable::SC_REGISTER;

	// functionContext
	SourceContext functionContext(context, SourceContext::CT_FUNCTION);

	// functionName
	std::string functionName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	// functionNameObject
	std::string functionNameObject(functionName);

	// functionLabel
	std::string functionLabel;
	if (token.getData() != "__extfunc")
		functionLabel += context->makeLabel();
	functionLabel += "function_";
	functionLabel += functionNameObject;

	// functionArgTypes/Names/Count functionReturn
	std::vector<VariableType const *> functionArgTypes;
	std::vector<std::string> functionArgNames;
	int functionArgCount;
	VariableType const * functionReturn;
	make_expression_arglist(in, blocks, context, &functionArgTypes, &functionArgNames, &functionArgCount, &functionContext, &functionReturn, functionArgClass);

	// functionVarType
	VariableType const * functionVarType(VariableType::get_function(functionReturn, functionArgTypes));

	// functionVariable
	// Before functionExpression to enable recursion.
	SourceVariable::Pointer functionVariable;
	if (target_type == TARGET_HexPP)
		functionVariable = SourceVariable::create_constant(functionName, functionVarType, functionLabel, token.getPosition());
	else
		functionVariable = SourceVariable::create_constant(functionName, functionVarType, functionNameObject, token.getPosition());
	context->addVariable(functionVariable);

	// functionExpression
	SourceExpression::Pointer functionExpression(make_expression_single(in, blocks, &functionContext));
	functionExpression->addLabel(functionLabel);
	blocks->push_back(functionExpression);
	blocks->push_back(create_branch_return(create_value_data(functionReturn, true, token.getPosition()), &functionContext, token.getPosition()));

	// functionVarCount
	int functionVarCount(functionContext.getLimit(functionArgClass));

	ObjectExpression::add_function(functionNameObject, functionLabel, functionArgCount, functionVarCount, functionReturn->size(token.getPosition()));

	return create_value_variable(functionVariable, token.getPosition());
}


