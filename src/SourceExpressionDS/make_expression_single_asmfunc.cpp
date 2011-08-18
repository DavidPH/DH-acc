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

/* SourceExpressionDS/make_expression_single_asmfunc.cpp
**
** Defines the SourceExpressionDS::make_expression_single_asmfunc function.
*/

#include "../SourceExpressionDS.hpp"

#include "../ObjectCode.hpp"
#include "../SourceContext.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"



SourceExpression::Pointer SourceExpressionDS::make_expression_single_asmfunc(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	// asmfuncName
	std::string asmfuncName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	// asmfuncOCode/Immediate
	ObjectCode asmfuncOCode(ocode_get_code(in->get(SourceTokenC::TT_IDENTIFIER)));
	ObjectCode asmfuncOCodeImmediate(ocode_get_code(in->get(SourceTokenC::TT_IDENTIFIER)));

	// asmfuncArgTypes asmfuncReturn
	std::vector<VariableType const *> asmfuncArgTypes;
	VariableType const * asmfuncReturn;
	make_expression_arglist(in, blocks, context, &asmfuncArgTypes, &asmfuncReturn);

	// asmfuncVarType
	VariableType const * asmfuncVarType(context->getVariableType_asmfunc(asmfuncReturn, asmfuncArgTypes));

	// asmfuncVarData
	SourceVariable::VariableData_AsmFunc asmfuncVarData = {asmfuncVarType, asmfuncOCode, asmfuncOCodeImmediate};

	// asmfuncVariable
	SourceVariable asmfuncVariable(asmfuncName, asmfuncVarData, token.getPosition());

	context->addVariable(asmfuncVariable);
	return create_value_variable(asmfuncVariable, token.getPosition());
}



