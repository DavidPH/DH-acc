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

/* SourceExpression/make_objects_call_native.cpp
**
** Defines the SourceExpression::make_objects_call_native function.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



void SourceExpression::make_objects_call_native(ObjectVector * objects, VariableType const * type, ObjectExpression * data, std::vector<SourceExpression::Pointer> const & args, SourcePosition const & position)
{
	if (args.size() != type->types.size())
		throw SourceException("incorrect arg count to call native", position, "SourceExpressionDS");

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i]->getType() != type->types[i])
			throw SourceException("incorrect arg type to call native", args[i]->position, "SourceExpressionDS");

		args[i]->makeObjectsGet(objects);
	}

	objects->setPosition(position);

	ObjectCode ocode(OCODE_CALLZDFUNC);
	ObjectExpression::Pointer oargc(objects->getValue((int)args.size()));
	ObjectExpression::Pointer ofunc(data);

	objects->addToken(ocode, oargc, ofunc);
}



