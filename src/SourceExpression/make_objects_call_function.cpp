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
#include "../ost_type.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



void SourceExpression::make_objects_call_function(ObjectVector * objects, VariableType const * type, ObjectExpression * data, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack, std::string const & labelReturn, SourcePosition const & position)
{
	if (args.size() != type->types.size())
		throw SourceException("incorrect arg count to call function", position, "SourceExpression");

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i]->getType() != type->types[i])
			throw SourceException("incorrect arg type to call function", args[i]->position, "SourceExpression");

		args[i]->makeObjectsGet(objects);
	}

	objects->setPosition(position);

	// Determine which OCODE to use.
	ObjectCode ocode;
	if (target_type != TARGET_ZDoom)
		ocode = OCODE_BRANCH_GOTO_IMM;
	else if (type->callType->vt == VariableType::VT_VOID)
		ocode = OCODE_ACSE_CALLFUNCVOID_IMM;
	else
		ocode = OCODE_ACSE_CALLFUNC_IMM;

	// Determine how many bytes of the return to handle.
	bigsint retnSize(type->callType->size(position));
	// ZDoom handles one of the return bytes for us.
	if (target_type == TARGET_ZDoom && retnSize >= 1)
		--retnSize;

	ObjectExpression::Pointer ostack(objects->getValueAdd(stack, retnSize));

	// Advance the stack-pointer.
	objects->addToken(OCODE_ADDR_STACK_ADD_IMM, ostack);

	// For not ZDoom...
	if (target_type != TARGET_ZDoom)
	{
		// ... Determine how many bytes of the call to handle.
		bigsint callSize(type->sizeCall(position));

		// ... Place args in auto vars.
		// FIXME: Should be based on type.
		for (bigsint i(callSize); i--;)
			objects->addToken(OCODE_SET_AUTO_VAR32I, objects->getValue(i));

		// ... Push return address.
		objects->addToken(OCODE_GET_LITERAL32I, ObjectExpression::create_value_symbol(labelReturn, position));
	}

	// The actual call. Data being the jump target.
	objects->addToken(ocode, data);
	objects->addLabel(labelReturn);

	// For any return bytes we're handling, push them onto the stack.
	// FIXME: Should be based on type.
	for (bigsint i(-retnSize); i; ++i)
		objects->addToken(OCODE_GET_AUTO_VAR32I, objects->getValue(i));

	// Reset the stack-pointer.
	objects->addToken(OCODE_ADDR_STACK_SUB_IMM, ostack);
}

void SourceExpression::make_objects_call_function(ObjectVector * objects, VariableType const * type, SourceExpression * data, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack, std::string const & labelReturn, SourcePosition const & position)
{
	if (args.size() != type->types.size())
		throw SourceException("incorrect arg count to call function", position, "SourceExpression");

	// Must push return address before target address.
	objects->addToken(OCODE_GET_LITERAL32I, ObjectExpression::create_value_symbol(labelReturn, position));

	// Determine jump target.
	data->makeObjectsGet(objects);

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i]->getType() != type->types[i])
			throw SourceException("incorrect arg type to call function", args[i]->position, "SourceExpression");

		args[i]->makeObjectsGet(objects);
	}

	objects->setPosition(position);

	// Determine which OCODE to use.
	ObjectCode ocode(OCODE_BRANCH_GOTO);

	// Determine how many bytes of the return to handle.
	bigsint retnSize(type->callType->size(position));

	ObjectExpression::Pointer ostack(objects->getValueAdd(stack, retnSize));

	// Advance the stack-pointer.
	objects->addToken(OCODE_ADDR_STACK_ADD_IMM, ostack);

	// Determine how many bytes of the call to handle.
	bigsint callSize(type->sizeCall(position));

	// Place args in auto vars.
	// FIXME: Should be based on type.
	for (bigsint i(callSize); i--;)
		objects->addToken(OCODE_SET_AUTO_VAR32I, objects->getValue(i));

	// The actual call.
	objects->addToken(ocode);
	objects->addLabel(labelReturn);

	// For any return bytes we're handling, push them onto the stack.
	// FIXME: Should be based on type.
	for (bigsint i(-retnSize); i; ++i)
		objects->addToken(OCODE_GET_AUTO_VAR32I, objects->getValue(i));

	// Reset the stack-pointer.
	objects->addToken(OCODE_ADDR_STACK_SUB_IMM, ostack);
}


