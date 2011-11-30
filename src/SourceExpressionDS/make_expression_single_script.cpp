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

/* SourceExpressionDS/make_expression_single_script.cpp
**
** Defines the SourceExpressionDS::make_expression_single_script function.
*/

#include "../SourceExpressionDS.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"
#include "../VariableType.hpp"

#include <sstream>



SRCEXPDS_EXPRSINGLE_DEFN(extern_script)
{
	// scriptName
	std::string scriptName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	// scriptArgTypes/Names/Count scriptReturn
	std::vector<VariableType const *> scriptArgTypes;
	int scriptArgCount;
	VariableType const * scriptReturn;
	make_expression_arglist(in, blocks, context, &scriptArgTypes, NULL, &scriptArgCount, NULL, &scriptReturn);

	// scriptLabel
	std::string scriptLabel("script_" + scriptName);

	// scriptNameObject
	std::string scriptNameObject(scriptLabel + "_id");

	// scriptVarType
	VariableType const * scriptVarType(VariableType::get_script(scriptReturn, scriptArgTypes));

	// scriptVariable
	SourceVariable::Pointer scriptVariable(SourceVariable::create_constant(scriptName, scriptVarType, scriptNameObject, token.getPosition()));
	context->addVariable(scriptVariable);

	return create_value_variable(scriptVariable, token.getPosition());
}

SRCEXPDS_EXPRSINGLE_DEFN(script)
{
	// scriptContext
	SourceContext scriptContext(context, SourceContext::CT_SCRIPT);

	// scriptName
	std::string scriptName(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	// scriptType
	ObjectData_Script::ScriptType scriptType;
	if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER)
	{
		SourceTokenC scriptTypeToken(in->get(SourceTokenC::TT_IDENTIFIER));
		scriptType = odata_get_ScriptType(scriptTypeToken);
	}
	else
	{
		scriptType = ObjectData_Script::ST_CLOSED;
	}

	// scriptFlags
	int scriptFlags(0);
	while (true)
	{
		if (in->peek().getType() != SourceTokenC::TT_IDENTIFIER)
			break;

		scriptFlags |= odata_get_ScriptFlag(in->get(SourceTokenC::TT_IDENTIFIER));
	}

	// scriptArgTypes/Names/Count scriptReturn
	std::vector<VariableType const *> scriptArgTypes;
	std::vector<std::string> scriptArgNames;
	int scriptArgCount;
	VariableType const * scriptReturn;
	make_expression_arglist(in, blocks, context, &scriptArgTypes, &scriptArgNames, &scriptArgCount, &scriptContext, &scriptReturn);

	// scriptNumber
	bigsint scriptNumber;
	if (in->peek().getType() == SourceTokenC::TT_OP_AT)
	{
		in->get(SourceTokenC::TT_OP_AT);
		scriptNumber = make_expression_single(in, blocks, context)->makeObject()->resolveInt();
	}
	else
	{
		scriptNumber = -1;
	}

	// scriptNameSource
	std::string scriptNameSource;
	if (scriptName == "auto")
	{
		if (scriptNumber < 0)
			throw SourceException("script name auto requires explicit script number", token.getPosition(), "SourceExpressionDS");

		std::ostringstream oss;
		oss << "script" << scriptNumber;
		scriptNameSource = oss.str();
	}
	else if (scriptName != "void")
	{
		scriptNameSource = scriptName;
	}

	// scriptLabel
	std::string scriptLabel;
	if (token.getData() != "__extscript")
		scriptLabel += context->makeLabel();
	scriptLabel += "script_";
	scriptLabel += scriptNameSource;

	// scriptNameObject
	std::string scriptNameObject(scriptLabel + "_id");

	// scriptVarType
	VariableType const * scriptVarType(VariableType::get_script(scriptReturn, scriptArgTypes));

	// scriptVariable
	// Before scriptExpression to enable recursion.
	SourceVariable::Pointer scriptVariable(SourceVariable::create_constant(scriptNameSource, scriptVarType, scriptNameObject, token.getPosition()));
	context->addVariable(scriptVariable);

	// scriptExpression
	SourceExpression::Pointer scriptExpression(create_root_script(scriptVarType, token.getPosition()));
	scriptExpression->addLabel(scriptLabel);
	blocks->push_back(scriptExpression);
	blocks->push_back(make_expression_single(in, blocks, &scriptContext));
	blocks->push_back(create_branch_return(create_value_data(scriptReturn, true, token.getPosition()), &scriptContext, token.getPosition()));

	// scriptVarCount
	int scriptVarCount(scriptContext.getLimit(SourceVariable::SC_REGISTER));

	if (scriptNumber < 0)
		ObjectExpression::add_script(scriptNameObject, scriptLabel, scriptType, scriptFlags, scriptArgCount, scriptVarCount);
	else
		ObjectExpression::add_script(scriptNameObject, scriptLabel, scriptType, scriptFlags, scriptArgCount, scriptVarCount, scriptNumber);

	return create_value_variable(scriptVariable, token.getPosition());
}


