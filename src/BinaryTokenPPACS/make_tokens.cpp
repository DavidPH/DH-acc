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

/* BinaryTokenPPACS/make_tokens.cpp
**
** Defines the BinaryTokenPPACS::make_tokens functions.
*/

#include "../BinaryTokenPPACS.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectToken.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"

#include "../BinaryTokenACS/make_tokens.hpp"



#define TOKEN_CLASS BinaryTokenPPACS



void BinaryTokenPPACS::make_tokens(ObjectToken const & object, std::vector<BinaryTokenPPACS> * instructions)
{
	static ObjectExpression::Pointer const fracbits(ObjectExpression::create_value_int(16, SourcePosition::builtin));

	static ObjectExpression::Pointer const indexTemp(ObjectExpression::create_value_int(1, SourcePosition::builtin));

	static std::vector<std::string> const nolabels;

	std::vector<ObjectExpression::Pointer> args;

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

	// Branching
	CASE_REMAP_PRE(BRANCH, GOTO,     GOTO);

	// Comparison

	// Logical

	// Stack-ops.

	// Variable Address
	CASE_REMAP_PRE(ADDR, STACK_ADD, STACK_ADD);
	CASE_REMAP_PRE(ADDR, STACK_SUB, STACK_SUB);
	CASE_REMAP_PRE(ADDR, STACK_VAR, STACK_VAR);

	// Variable Get
	CASE_REMAP_PRE(GET, AUTO_VAR32F, AUTO_VAR);
	CASE_REMAP_PRE(GET, AUTO_VAR32I, AUTO_VAR);

	CASE_REMAP_PRE(GET, MOBJ_VAR32F, MOBJ_VAR);
	CASE_REMAP_PRE(GET, MOBJ_VAR32I, MOBJ_VAR);

	CASE_REMAP_PRE(GET, POINTER_VAR32F, POINTER_VAR);
	CASE_REMAP_PRE(GET, POINTER_VAR32I, POINTER_VAR);

	CASE_REMAP_PRE(GET, STATIC_VAR32F, STATIC_VAR);
	CASE_REMAP_PRE(GET, STATIC_VAR32I, STATIC_VAR);

	// Variable Set
	CASE_REMAP_PRE(SET, AUTO_VAR32F, AUTO_VAR);
	CASE_REMAP_PRE(SET, AUTO_VAR32I, AUTO_VAR);

	CASE_REMAP_PRE(SET, MOBJ_VAR32F, MOBJ_VAR);
	CASE_REMAP_PRE(SET, MOBJ_VAR32I, MOBJ_VAR);

	CASE_REMAP_PRE(SET, POINTER_VAR32F, POINTER_VAR);
	CASE_REMAP_PRE(SET, POINTER_VAR32I, POINTER_VAR);

	CASE_REMAP_PRE(SET, STATIC_VAR32F, STATIC_VAR);
	CASE_REMAP_PRE(SET, STATIC_VAR32I, STATIC_VAR);

	// Miscellaneous

	// ACS

	// ACS Common Extensions

	// ACS Extensions

	// ACS Printing
	CASE_MAP_ACSP(END_ERROR);
	CASE_MAP_ACSP(END_LOG);
	CASE_REMAP_ACSP(NUM_DEC32F, FIXED);



	// Translations


	BINTOKACS_TOKENS_TRAN_ALL_ALL();

	// Variable Address

	case OCODE_ADDR_STACK_ADD_IMM:
		PUSH_TOKEN_ARGS1(BCODE_GET_LITERAL, 1);
		PUSH_TOKEN(BCODE_ADDR_STACK_SUB);
		break;

	case OCODE_ADDR_STACK_SUB_IMM:
		PUSH_TOKEN_ARGS1(BCODE_GET_LITERAL, 1);
		PUSH_TOKEN(BCODE_ADDR_STACK_SUB);
		break;

	// Variable Get

	case OCODE_GET_POINTERTEMP_VAR32F:
	case OCODE_GET_POINTERTEMP_VAR32I:
		args.push_back(indexTemp);
		PUSH_TOKEN(BCODE_GET_WORLDREGISTER_VAR);
		PUSH_TOKEN_ARGS1(BCODE_GET_POINTER_VAR, 1);
		break;

	// Variable Set

	case OCODE_SET_POINTERTEMP_VAR32F:
	case OCODE_SET_POINTERTEMP_VAR32I:
		args.push_back(indexTemp);
		PUSH_TOKEN(BCODE_GET_WORLDREGISTER_VAR);
		PUSH_TOKEN_ARGS1(BCODE_SET_POINTER_VAR, 1);
		break;

	case OCODE_SET_TEMP_VAR:
		args.push_back(indexTemp);
		PUSH_TOKEN(BCODE_SET_WORLDREGISTER_VAR);
		break;

	case OCODE_NONE:
	default:
		throw SourceException("unknown OCODE: " + (std::string)make_string(object.getCode()), position, "BinaryTokenPPACS");
	}
}
void BinaryTokenPPACS::make_tokens(ObjectVector const & objects, std::vector<BinaryTokenPPACS> * instructions)
{
	for (bigsint index(0); index < objects.size(); ++index)
		make_tokens(objects[index], instructions);
}


