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

/* BinaryTokenZDACS/make_tokens.cpp
**
** Defines the BinaryTokenZDACS::make_tokens functions.
*/

#include "../BinaryTokenZDACS.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectToken.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"

#include "../BinaryTokenACS/make_tokens.hpp"



#define TOKEN_CLASS BinaryTokenZDACS



void BinaryTokenZDACS::make_tokens(ObjectToken const & object, std::vector<BinaryTokenZDACS> * instructions)
{
	static ObjectExpression::Pointer const fracbits(ObjectExpression::create_value_int(16, SourcePosition::builtin()));

	static ObjectExpression::Pointer const indexAddr(ObjectExpression::create_value_int(0, SourcePosition::builtin()));
	static ObjectExpression::Pointer const indexStack(ObjectExpression::create_value_int(0, SourcePosition::builtin()));
	static ObjectExpression::Pointer const indexTemp(ObjectExpression::create_value_int(1, SourcePosition::builtin()));

	static std::vector<std::string> const nolabels;

	std::vector<ObjectExpression::Pointer> args;

	BinaryCode bcode;

	SourcePosition const & position(object.getPosition());

	std::vector<std::string> const * labels(&object.getLabels());

	switch (object.getCode())
	{
	// Direct Mappings


	BINTOKACS_TOKENS_MAP_ALL_ALL();

	// Arithmetic
	CASE_REMAP(DIV32F, DIV_FIXED);
	CASE_REMAP(MUL32F, MUL_FIXED);

	// Bitwise
	CASE_REMAP_PRE(BITWISE, NOT32, NOT);

	// Branching

	// Comparison

	// Logical

	// Stack-ops.
	CASE_REMAP_PRE(STACK, DUP32,  DUP);
	CASE_REMAP_PRE(STACK, SWAP32, SWAP);

	// Variable Address

	// Variable Get
	CASE_REMAP_PRE(GET, GLOBALARRAY_VAR32F, GLOBALARRAY_VAR);
	CASE_REMAP_PRE(GET, GLOBALARRAY_VAR32I, GLOBALARRAY_VAR);

	CASE_REMAP_PRE(GET, GLOBALREGISTER_VAR32F, GLOBALREGISTER_VAR);
	CASE_REMAP_PRE(GET, GLOBALREGISTER_VAR32I, GLOBALREGISTER_VAR);

	CASE_REMAP_PRE(GET, MAPARRAY_VAR32F, MAPARRAY_VAR);
	CASE_REMAP_PRE(GET, MAPARRAY_VAR32I, MAPARRAY_VAR);

	CASE_REMAP_PRE(GET, TID_VAR32F, TID_VAR);
	CASE_REMAP_PRE(GET, TID_VAR32I, TID_VAR);

	CASE_REMAP_PRE(GET, WORLDARRAY_VAR32F, WORLDARRAY_VAR);
	CASE_REMAP_PRE(GET, WORLDARRAY_VAR32I, WORLDARRAY_VAR);

	// Variable Set
	CASE_REMAP_PRE(SET, GLOBALARRAY_VAR32F, GLOBALARRAY_VAR);
	CASE_REMAP_PRE(SET, GLOBALARRAY_VAR32I, GLOBALARRAY_VAR);

	CASE_REMAP_PRE(SET, GLOBALREGISTER_VAR32F, GLOBALREGISTER_VAR);
	CASE_REMAP_PRE(SET, GLOBALREGISTER_VAR32I, GLOBALREGISTER_VAR);

	CASE_REMAP_PRE(SET, MAPARRAY_VAR32F, MAPARRAY_VAR);
	CASE_REMAP_PRE(SET, MAPARRAY_VAR32I, MAPARRAY_VAR);

	CASE_REMAP_PRE(SET, TID_VAR32F, TID_VAR);
	CASE_REMAP_PRE(SET, TID_VAR32I, TID_VAR);

	CASE_REMAP_PRE(SET, WORLDARRAY_VAR32F, WORLDARRAY_VAR);
	CASE_REMAP_PRE(SET, WORLDARRAY_VAR32I, WORLDARRAY_VAR);

	// Variable Set Op

	// Miscellaneous
	CASE_REMAP_PRE(MISC, NATIVE, NATIVE);

	CASE_REMAP_PRE(MISC, STRLEN, STRLEN);

	// ACS

	// ACS Common Extensions

	// ACS Extensions
	CASE_MAP_ACSE(CALLFUNC_IMM);
	CASE_MAP_ACSE(CALLFUNCVOID_IMM);
	CASE_MAP_ACSE(GET_ACTIVATOR_TID);
	CASE_MAP_ACSE(GET_AMMO_CAPACITY);
	CASE_MAP_ACSE(GET_TID_ANGLE);
	CASE_MAP_ACSE(GET_TID_PITCH);
	CASE_MAP_ACSE(GET_TID_X);
	CASE_MAP_ACSE(GET_TID_Y);
	CASE_MAP_ACSE(GET_TID_Z);
	CASE_MAP_ACSE(INVENTORY_CHECK);
	CASE_MAP_ACSE(INVENTORY_CHECK_IMM);
	CASE_MAP_ACSE(INVENTORY_GIVE);
	CASE_MAP_ACSE(INVENTORY_GIVE_IMM);
	CASE_MAP_ACSE(INVENTORY_TAKE);
	CASE_MAP_ACSE(INVENTORY_TAKE_IMM);
	CASE_MAP_ACSE(LINESPEC5RESULT);
	CASE_MAP_ACSE(RETNFUNC);
	CASE_MAP_ACSE(RETNFUNCVOID);
	CASE_MAP_ACSE(SCRIPT_SETRETURN);
	CASE_MAP_ACSE(SET_AMMO_CAPACITY);
	CASE_MAP_ACSE(SET_TID_ANGLE);
	CASE_MAP_ACSE(SET_TID_PITCH);
	CASE_MAP_ACSE(SET_TID_POSITION);
	CASE_MAP_ACSE(SPAWN);
	CASE_MAP_ACSE(SPAWN_IMM);
	CASE_MAP_ACSE(SPAWN_PROJECTILE);

	// ACS Printing
	CASE_MAP_ACSP(END_LOG);
	CASE_REMAP_ACSP(NUM_DEC32F, FIXED);
	CASE_REMAP_ACSP(NUM_HEX32I, NUM_HEX); // WARNING
	CASE_REMAP_ACSP(NUM_HEX32U, NUM_HEX);



	// Translations


	BINTOKACS_TOKENS_TRAN_ALL();

	// Branching

	case OCODE_BRANCH_TABLE:
		args = object.getArgs();
		if (args.size() % 2)
			throw SourceException("uneven OCODE_BRANCH_TABLE", position, "BinaryTokenZDACS");
		PUSH_TOKEN(BCODE_BRANCH_TABLE);
		break;

	// Variable Address

	case OCODE_ADDR_STACK_ADD_IMM:
		PUSH_TOKEN_ARGS1(BCODE_GET_LITERAL, 1);
	case OCODE_ADDR_STACK_ADD:
		args.push_back(indexStack);
		PUSH_TOKEN(BCODE_SETOP_ADD_WORLDREGISTER_VAR);
		break;

	case OCODE_ADDR_STACK_SUB_IMM:
		PUSH_TOKEN_ARGS1(BCODE_GET_LITERAL, 1);
	case OCODE_ADDR_STACK_SUB:
		args.push_back(indexStack);
		PUSH_TOKEN(BCODE_SETOP_SUB_WORLDREGISTER_VAR);
		break;

	case OCODE_ADDR_STACK_VAR:
		PUSH_TOKEN_ARGS1(BCODE_GET_LITERAL, 1);
		args.push_back(indexStack);
		PUSH_TOKEN(BCODE_GET_WORLDREGISTER_VAR);
		PUSH_TOKEN(BCODE_ADD);
		break;

	// Variable Get

	case OCODE_GET_AUTO_VAR32F:
	case OCODE_GET_AUTO_VAR32I:
		PUSH_TOKEN_ARGS1(BCODE_GET_LITERAL, 1);
		args.push_back(indexStack);
		PUSH_TOKEN(BCODE_GET_WORLDREGISTER_VAR);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(indexAddr);
		PUSH_TOKEN(BCODE_GET_GLOBALARRAY_VAR);
		break;

	get_array_case:
		PUSH_TOKEN_ARGS2(BCODE_GET_LITERAL, 1, 2);
		args.push_back(indexTemp);
		PUSH_TOKEN(BCODE_GET_WORLDREGISTER_VAR);
		PUSH_TOKEN(BCODE_ADD);
		PUSH_TOKEN_ARGS2(bcode, 0, 1);
		break;

	case OCODE_GET_GLOBALARRAYTEMP_VAR32F:
	case OCODE_GET_GLOBALARRAYTEMP_VAR32I:
		bcode = BCODE_GET_GLOBALARRAY_VAR;
		goto get_array_case;

	case OCODE_GET_MAPARRAYTEMP_VAR32F:
	case OCODE_GET_MAPARRAYTEMP_VAR32I:
		bcode = BCODE_GET_MAPARRAY_VAR;
		goto get_array_case;

	case OCODE_GET_POINTERTEMP_VAR32F:
	case OCODE_GET_POINTERTEMP_VAR32I:
		args.push_back(indexTemp);
		PUSH_TOKEN(BCODE_GET_WORLDREGISTER_VAR);
	case OCODE_GET_POINTER_VAR32F:
	case OCODE_GET_POINTER_VAR32I:
		if (object.getArg(0)->resolveInt())
		{
			PUSH_TOKEN_ARGS1(BCODE_GET_LITERAL, 1);
			PUSH_TOKEN(BCODE_ADD);
		}
		args.push_back(indexAddr);
		PUSH_TOKEN(BCODE_GET_GLOBALARRAY_VAR);
		break;

	case OCODE_GET_STATIC_VAR32F:
	case OCODE_GET_STATIC_VAR32I:
		args.push_back(ObjectExpression::create_binary_add(object.getArg(0), ObjectExpression::static_offset, SourcePosition::none()));
		PUSH_TOKEN(BCODE_GET_LITERAL);
		args.push_back(indexAddr);
		PUSH_TOKEN(BCODE_GET_GLOBALARRAY_VAR);
		break;

	case OCODE_GET_WORLDARRAYTEMP_VAR32F:
	case OCODE_GET_WORLDARRAYTEMP_VAR32I:
		bcode = BCODE_GET_WORLDARRAY_VAR;
		goto get_array_case;

	// Variable Set

	case OCODE_SET_AUTO_VAR32F:
	case OCODE_SET_AUTO_VAR32I:
		PUSH_TOKEN_ARGS1(BCODE_GET_LITERAL, 1);
		args.push_back(indexStack);
		PUSH_TOKEN(BCODE_GET_WORLDREGISTER_VAR);
		PUSH_TOKEN(BCODE_ADD);
		PUSH_TOKEN(BCODE_STACK_SWAP);
		args.push_back(indexAddr);
		PUSH_TOKEN(BCODE_SET_GLOBALARRAY_VAR);
		break;

	set_array_case:
		PUSH_TOKEN_ARGS2(BCODE_GET_LITERAL, 1, 2);
		args.push_back(indexTemp);
		PUSH_TOKEN(BCODE_GET_WORLDREGISTER_VAR);
		PUSH_TOKEN(BCODE_ADD);
		PUSH_TOKEN(BCODE_STACK_SWAP);
		PUSH_TOKEN_ARGS2(bcode, 0, 1);
		break;

	case OCODE_SET_GLOBALARRAYTEMP_VAR32F:
	case OCODE_SET_GLOBALARRAYTEMP_VAR32I:
		bcode = BCODE_SET_GLOBALARRAY_VAR;
		goto set_array_case;

	case OCODE_SET_MAPARRAYTEMP_VAR32F:
	case OCODE_SET_MAPARRAYTEMP_VAR32I:
		bcode = BCODE_SET_MAPARRAY_VAR;
		goto set_array_case;

	case OCODE_SET_POINTERTEMP_VAR32F:
	case OCODE_SET_POINTERTEMP_VAR32I:
		args.push_back(indexTemp);
		PUSH_TOKEN(BCODE_GET_WORLDREGISTER_VAR);
	case OCODE_SET_POINTER_VAR32F:
	case OCODE_SET_POINTER_VAR32I:
		if (object.getArg(0)->resolveInt())
		{
			PUSH_TOKEN_ARGS1(BCODE_GET_LITERAL, 1);
			PUSH_TOKEN(BCODE_ADD);
		}
		PUSH_TOKEN(BCODE_STACK_SWAP);
		args.push_back(indexAddr);
		PUSH_TOKEN(BCODE_SET_GLOBALARRAY_VAR);
		break;

	case OCODE_SET_STATIC_VAR32F:
	case OCODE_SET_STATIC_VAR32I:
		args.push_back(ObjectExpression::create_binary_add(object.getArg(0), ObjectExpression::static_offset, SourcePosition::none()));
		PUSH_TOKEN(BCODE_GET_LITERAL);
		PUSH_TOKEN(BCODE_STACK_SWAP);
		args.push_back(indexAddr);
		PUSH_TOKEN(BCODE_SET_GLOBALARRAY_VAR);
		break;

	case OCODE_SET_TEMP_VAR:
		args.push_back(indexTemp);
		PUSH_TOKEN(BCODE_SET_WORLDREGISTER_VAR);
		break;

	case OCODE_SET_WORLDARRAYTEMP_VAR32F:
	case OCODE_SET_WORLDARRAYTEMP_VAR32I:
		bcode = BCODE_SET_WORLDARRAY_VAR;
		goto set_array_case;

	case OCODE_NONE:
	default:
		throw SourceException("unknown OCODE: " + (std::string)make_string(object.getCode()), position, "BinaryTokenZDACS");
	}
}
void BinaryTokenZDACS::make_tokens(ObjectVector const & objects, std::vector<BinaryTokenZDACS> * instructions)
{
	for (bigsint index(0); index < objects.size(); ++index)
		make_tokens(objects[index], instructions);
}


