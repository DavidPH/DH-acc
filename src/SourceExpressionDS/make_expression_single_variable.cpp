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

/* SourceExpressionDS/make_expression_single_var.cpp
**
** Defines the SourceExpressionDS::make_expression_single_var function.
*/

#include "../SourceExpressionDS.hpp"

#include "../SourceContext.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerDS.hpp"



SRCEXPDS_EXPRSINGLE_DEFN(extern_variable)
{
	SourceVariable::StorageClass sc(SourceVariable::get_StorageClass(in->get(SourceTokenC::TT_IDENTIFIER)));
	VariableType const * type(make_expression_type(in, blocks, context));
	std::string name(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	SourceVariable::Pointer var(SourceVariable::create_variable(name, type, name, sc, token.getPosition()));

	context->addVariable(var);

	return create_value_variable(var, token.getPosition());
}
SRCEXPDS_EXPRSINGLE_DEFN(variable)
{
	bool external(token.getData() == "__extvar");

	SourceVariable::StorageClass sc(SourceVariable::get_StorageClass(in->get(SourceTokenC::TT_IDENTIFIER)));
	VariableType const * type(make_expression_type(in, blocks, context));
	std::string name(in->get(SourceTokenC::TT_IDENTIFIER).getData());

	std::string nameObject;
	if (in->peek().getType() == SourceTokenC::TT_OP_AT)
	{
		in->get(SourceTokenC::TT_OP_AT);
		bigsint address = get_bigsint(in->get(SourceTokenC::TT_INTEGER));
		nameObject = context->makeNameObject(external, sc, type, name, address, token.getPosition());
	}
	else
	{
		nameObject = context->makeNameObject(external, sc, type, name, token.getPosition());
	}

	SourceVariable::Pointer var(SourceVariable::create_variable(name, type, nameObject, sc, token.getPosition()));

	context->addVariable(var);

	return create_value_variable(var, token.getPosition());
}


