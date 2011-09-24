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

/* SourceExpressionDS/make_expression_single_extern.cpp
**
** Defines the SourceExpressionDS::make_expression_single_extern function.
*/

#include "../SourceExpressionDS.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableType.hpp"



SourceExpression::Pointer SourceExpressionDS::make_expression_single_extern(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceTokenC externToken(in->get(SourceTokenC::TT_IDENTIFIER));

	if (externToken.getData() == "function")
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
		SourceVariable::Pointer functionVariable(SourceVariable::create_constant(functionName, functionVarType, functionNameObject, token.getPosition()));

		context->addVariable(functionVariable);
		ObjectExpression::add_function(functionNameObject, functionLabel, functionArgCount, functionArgCount, functionReturn->size(token.getPosition()), "");
		return create_value_variable(functionVariable, token.getPosition());
	}
	else
		throw SourceException("unknown extern type", externToken.getPosition(), "SourceExpressionDS::make_expression_single_extern");
}


