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

/* SourceExpression/make_objects_call_function.cpp
**
** Defines the SourceExpression::make_objects_call_function function.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



void SourceExpression::make_objects_call_function(ObjectVector * objects, SourceVariable::VariableData_Function const & data, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack, SourcePosition const & position)
{
	if (args.size() != data.type->types.size())
		throw SourceException("incorrect arg count to call function", position, "SourceExpressionDS");

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i]->getType() != data.type->types[i])
			throw SourceException("incorrect arg type to call function", args[i]->position, "SourceExpressionDS");

		args[i]->makeObjectsGet(objects);
	}

	objects->setPosition(position);

	ObjectCode ocode;
	ObjectExpression::Pointer ofunc(objects->getValue(data.number));
	ObjectExpression::Pointer oretn;

	if (data.type->callType->vt == VariableType::VT_VOID)
		ocode = OCODE_CALLZDACSDISCARD;
	else
		ocode = OCODE_CALLZDACS;

	if (data.type->callType->size() > 1)
		oretn = objects->getValue(data.type->callType->size() - 1);

	objects->addToken(OCODE_ADDSTACK_IMM, stack);
	if (oretn) objects->addToken(OCODE_ADDSTACK_IMM, oretn);
	objects->addToken(ocode, ofunc);
	if (oretn)
	{
		for (int i(-data.type->callType->size()); ++i;)
			objects->addToken(OCODE_PUSHSTACKVAR, objects->getValue(i));

		objects->addToken(OCODE_SUBSTACK_IMM, oretn);
	}
	objects->addToken(OCODE_SUBSTACK_IMM, stack);
}



