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

/* SourceExpression/make_objects_call_linespec.cpp
**
** Defines the SourceExpression::make_objects_call_linespec function.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



void SourceExpression::make_objects_call_linespec(ObjectVector * objects, SourceVariable::VariableData_LineSpec const & data, std::vector<SourceExpression::Pointer> const & args, SourcePosition const & position)
{
	if (data.type->sizeCall() > 5)
		throw SourceException("too many args to call linespec", position, "SourceExpression");

	if (args.size() != data.type->types.size())
		throw SourceException("incorrect arg count to call linespec", position, "SourceExpression");

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i]->getType() != data.type->types[i])
			throw SourceException("incorrect arg type to call lnpsec", args[i]->position, "SourceExpression");

		args[i]->makeObjectsGet(objects);
	}

	objects->setPosition(position);

	ObjectCode ocode;
	ObjectExpression::Pointer ospec(objects->getValue(data.number));

	if (data.type->callType->vt == VariableType::VT_VOID)
	{
		switch (data.type->sizeCall())
		{
		case 0: ocode = OCODE_LSPEC1; objects->addTokenPushZero(); break;
		case 1: ocode = OCODE_LSPEC1; break;
		case 2: ocode = OCODE_LSPEC2; break;
		case 3: ocode = OCODE_LSPEC3; break;
		case 4: ocode = OCODE_LSPEC4; break;
		case 5: ocode = OCODE_LSPEC5; break;
		default: throw SourceException("unexpected arg count to call linespec", position, "SourceExpression");
		}
	}
	else
	{
		ocode = OCODE_LSPEC5RESULT;

		for (size_t i(data.type->sizeCall()); i < 5; ++i)
			objects->addTokenPushZero();
	}

	objects->addToken(ocode, ospec);
}



