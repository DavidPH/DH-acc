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

/* SourceExpressionDS/make_expression_type.cpp
**
** Defines the SourceExpressionDS::make_expression_type function.
*/

#include "../SourceExpressionDS.hpp"

#include "../SourceTokenizerDS.hpp"



SourceVariable::VariableType const * SourceExpressionDS::make_expression_type(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	SourceTokenC token(in->get(SourceTokenC::TT_IDENTIFIER));

	std::string name;
	std::vector<std::string> names;
	SourceVariable::VariableType const * retn;
	SourceVariable::VariableType const * type;
	std::vector<SourceVariable::VariableType const *> types;

	if (token.getData() == "array")
	{
		SourceVariable::VariableType const * refType(make_expression_type(in, blocks, context));
		in->get(SourceTokenC::TT_OP_BRACKET_O);
		int count((int)make_expression(in, blocks, context)->makeObject()->resolveInt());
		in->get(SourceTokenC::TT_OP_BRACKET_C);
		type = SourceVariable::get_VariableType_array(refType, count);
	}
	else if (token.getData() == "block")
	{
		in->get(SourceTokenC::TT_OP_BRACE_O);

		while (true)
		{
			if (in->peek().getType() == SourceTokenC::TT_OP_BRACE_C)
				break;

			types.push_back(make_expression_type(in, blocks, context));

			in->get(SourceTokenC::TT_OP_SEMICOLON);
		}

		in->get(SourceTokenC::TT_OP_BRACE_C);

		type = SourceVariable::get_VariableType_block(types);
	}
	else if (token.getData() == "script")
	{
		make_expression_arglist(in, blocks, context, &types, &retn);

		type = SourceVariable::get_VariableType_script(retn, types);
	}
	else if (token.getData() == "struct")
	{
		if (in->peek()->getType() == SourceTokenC::TT_IDENTIFIER)
			name = in->get(SourceTokenC::TT_IDENTIFIER).getData();

		in->get(SourceTokenC::TT_OP_BRACE_O);

		while (true)
		{
			if (in->peek().getType() == SourceTokenC::TT_OP_BRACE_C)
				break;

			types.push_back(make_expression_type(in, blocks, context));
			names.push_back(in->get(SourceTokenC::TT_IDENTIFIER).getData());

			in->get(SourceTokenC::TT_OP_SEMICOLON);
		}

		in->get(SourceTokenC::TT_OP_BRACE_C);

		type = SourceVariable::add_struct(name, names, types);
	}
	else if (token.getData() == "typeof")
	{
		type = make_expression_single(in, blocks, context)->getType();
	}
	else
	{
		type = SourceVariable::get_VariableType(token);
	}

	return type;
}



