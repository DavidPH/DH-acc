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
#include "../VariableData.hpp"
#include "../VariableType.hpp"



void SourceExpression::make_objects_call_linespec(ObjectVector *objects, VariableData *dst, VariableType const *type, ObjectExpression *data, std::vector<SourceExpression::Pointer> const &args, SourcePosition const &position)
{
	if (type->sizeCall(position) > 5)
		throw SourceException("too many args to call linespec", position, "SourceExpression");

	if (args.size() != type->types.size())
		throw SourceException("incorrect arg count to call linespec", position, "SourceExpression");

	VariableData::Pointer src = VariableData::create_stack(type->callType->size(position));

	make_objects_memcpy_prep(objects, dst, src, position);

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i]->getType() != type->types[i])
			throw SourceException("incorrect arg type to call lnpsec", args[i]->position, "SourceExpression");

		args[i]->makeObjects(objects, VariableData::create_stack(args[i]->getType()->size(position)));
	}

	objects->setPosition(position);

	ObjectCode ocode;
	ObjectExpression::Pointer ospec(data);

	if (type->callType->vt == VariableType::VT_VOID)
	{
		switch (type->sizeCall(position))
		{
		case 0: ocode = OCODE_ACS_LINESPEC1; objects->addTokenPushZero(); break;
		case 1: ocode = OCODE_ACS_LINESPEC1; break;
		case 2: ocode = OCODE_ACS_LINESPEC2; break;
		case 3: ocode = OCODE_ACS_LINESPEC3; break;
		case 4: ocode = OCODE_ACS_LINESPEC4; break;
		case 5: ocode = OCODE_ACS_LINESPEC5; break;
		default: throw SourceException("unexpected arg count to call linespec", position, "SourceExpression");
		}
	}
	else
	{
		ocode = OCODE_ACSE_LINESPEC5RESULT;

		for (size_t i(type->sizeCall(position)); i < 5; ++i)
			objects->addTokenPushZero();
	}

	objects->addToken(ocode, ospec);

	make_objects_memcpy_post(objects, dst, src, position);
}


