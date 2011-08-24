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

/* SourceExpression/make_objects_call_asmfunc.cpp
**
** Defines the SourceExpression::make_objects_call_asmfunc function.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



void SourceExpression::make_objects_call_asmfunc(ObjectVector * objects, VariableType const * type, ObjectExpression * data, std::vector<SourceExpression::Pointer> const & args, SourcePosition const & position)
{
	if (args.size() != type->types.size())
		throw SourceException("incorrect arg count to call asmfunc", position, "SourceExpressionDS");

	ObjectCodeSet ocode(data->resolveOCode());

	bool immediate(ocode.ocode_imm != OCODE_NONE);

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i]->getType() != type->types[i])
			throw SourceException("incorrect arg type to call asmfunc", args[i]->position, "SourceExpressionDS");

		immediate = immediate && args[i]->canMakeObject();
	}

	if (immediate)
	{
		std::vector<ObjectExpression::Pointer> oargs;

		for (size_t i(0); i < args.size(); ++i)
			oargs.push_back(args[i]->makeObject());

		objects->setPosition(position).addToken(ocode.ocode_imm, oargs);
	}
	else
	{
		if (ocode.ocode == OCODE_NONE)
			throw SourceException("no ocode", position, "SourceExpressionDS");

		for (size_t i(0); i < args.size(); ++i)
			args[i]->makeObjectsGet(objects);

		objects->setPosition(position).addToken(ocode.ocode);
	}
}



