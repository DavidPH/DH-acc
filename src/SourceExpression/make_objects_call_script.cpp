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

/* SourceExpression/make_objects_call_script.cpp
**
** Defines the SourceExpression::make_objects_call_script function.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::make_objects_call_script
//
void SourceExpression::make_objects_call_script(ObjectVector *objects, VariableData *dst, VariableType const *type, SourceExpression *data, std::vector<SourceExpression::Pointer> const &args, ObjectExpression *stack, SourcePosition const &position)
{
	if (args.size() != type->types.size())
		throw SourceException("incorrect arg count to call script", position, "SourceExpression");

	VariableData::Pointer src = VariableData::create_stack(type->callType->size(position));

	make_objects_memcpy_prep(objects, dst, src, position);

	data->makeObjects(objects, VariableData::create_stack(type->size(position)));

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i]->getType() != type->types[i])
			throw SourceException("incorrect arg type to call script", args[i]->position, "SourceExpression");

		args[i]->makeObjects(objects, VariableData::create_stack(args[i]->getType()->size(position)));
	}

	objects->setPosition(position);

	ObjectCode ocode;
	ObjectExpression::Pointer ospec(objects->getValue(84));
	ObjectExpression::Pointer oretn;

	if (type->callType->vt == VariableType::VT_VOID)
	{
		switch (type->sizeCall(position))
		{
      case  0: ocode = OCODE_ACS_LINE_SPEC_EXEC1; break;
      case  1: ocode = OCODE_ACS_LINE_SPEC_EXEC2; break;
      case  2: ocode = OCODE_ACS_LINE_SPEC_EXEC3; break;
      case  3: ocode = OCODE_ACS_LINE_SPEC_EXEC4; break;
      default: ocode = OCODE_ACS_LINE_SPEC_EXEC4; break;
		}
	}
	else
	{
		ocode = OCODE_ACSE_LINESPEC5RESULT;

		// Dummy args.
		for (size_t i(type->sizeCall(position)); i < 3; ++i)
			objects->addTokenPushZero();
	}

	// Extended return data?
	if (type->callType->size(position) > 1)
		oretn = objects->getValue(type->callType->size(position) - 1);

	// Stack for call.
	objects->addToken(OCODE_ADDR_STACK_ADD_IMM, stack);

	// Stack for extended return data.
	if (oretn) objects->addToken(OCODE_ADDR_STACK_ADD_IMM, oretn);

	// Extended call data.
	// FIXME: Should be based on type.
	if (type->sizeCall(position) > 3) for (int i(type->sizeCall(position) - 3); i--;)
		objects->addToken(OCODE_SET_AUTO_VAR32I, objects->getValue(i));

	// Dummy arg.
	if (ocode == OCODE_ACSE_LINESPEC5RESULT) objects->addTokenPushZero();

	// Call.
	objects->addToken(ocode, ospec);

	// Extended return data.
	if (oretn)
	{
		// FIXME: Should be based on type.
		for (int i(-type->callType->size(position)); ++i;)
			objects->addToken(OCODE_GET_AUTO_VAR32I, objects->getValue(i));

		objects->addToken(OCODE_ADDR_STACK_SUB_IMM, oretn);
	}

	// Stack for call.
	objects->addToken(OCODE_ADDR_STACK_SUB_IMM, stack);

	make_objects_memcpy_post(objects, dst, VariableData::create_stack(type->callType->size(position)), position);
}

// EOF

