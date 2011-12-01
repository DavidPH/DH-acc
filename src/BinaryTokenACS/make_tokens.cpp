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

/* BinaryTokenACS/make_tokens.cpp
**
** Defines the BinaryTokenACS::make_tokens functions.
*/

#include "../BinaryTokenACS.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectToken.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"

#include "../BinaryToken/make_tokens_macros.hpp"



#define TOKEN_CLASS BinaryTokenACS



void BinaryTokenACS::make_tokens(ObjectToken const & object, std::vector<BinaryTokenACS> * instructions)
{
	static ObjectExpression::Pointer const fracbits(ObjectExpression::create_value_int(16, SourcePosition::builtin));

	static std::vector<std::string> const nolabels;

	std::vector<ObjectExpression::Pointer> args;

	SourcePosition const & position(object.getPosition());

	std::vector<std::string> const * labels(&object.getLabels());

	switch (object.getCode())
	{
	// Direct Mappings


	CASE_REMAP(NOP, NOP);

	// Arithmetic
	CASE_REMAP(ADD32F, ADD);
	CASE_REMAP(ADD32I, ADD);
	CASE_REMAP(ADD32U, ADD);

	CASE_REMAP(DIV32I, DIV);
	CASE_REMAP(DIV32U, DIV); // WARNING

	CASE_REMAP(MOD32F, MOD);
	CASE_REMAP(MOD32I, MOD);
	CASE_REMAP(MOD32U, MOD); // WARNING

	CASE_REMAP(MUL32I, MUL);
	CASE_REMAP(MUL32U, MUL);

	CASE_REMAP(SUB32F, SUB);
	CASE_REMAP(SUB32I, SUB);
	CASE_REMAP(SUB32U, SUB);

	// Bitwise
	CASE_REMAP_PRE(BITWISE, AND32,    AND);
	CASE_REMAP_PRE(BITWISE, IOR32,    IOR);
	CASE_REMAP_PRE(BITWISE, SHIFTL32, SHIFTL);
	CASE_REMAP_PRE(BITWISE, SHIFTR32, SHIFTR);
	CASE_REMAP_PRE(BITWISE, XOR32,    XOR);

	// Branching
	CASE_REMAP_PRE(BRANCH, CASE,     CASE);
	CASE_REMAP_PRE(BRANCH, GOTO_IMM, GOTO_IMM);
	CASE_REMAP_PRE(BRANCH, TRUE,     TRUE);
	CASE_REMAP_PRE(BRANCH, ZERO,     ZERO);

	// Comparison
	CASE_REMAP_PRE(CMP, EQ32F, EQ);
	CASE_REMAP_PRE(CMP, EQ32I, EQ);

	CASE_REMAP_PRE(CMP, GE32F, GE);
	CASE_REMAP_PRE(CMP, GE32I, GE);
	CASE_REMAP_PRE(CMP, GE32U, GE); // WARNING

	CASE_REMAP_PRE(CMP, GT32F, GT);
	CASE_REMAP_PRE(CMP, GT32I, GT);
	CASE_REMAP_PRE(CMP, GT32U, GT); // WARNING

	CASE_REMAP_PRE(CMP, LE32F, LE);
	CASE_REMAP_PRE(CMP, LE32I, LE);
	CASE_REMAP_PRE(CMP, LE32U, LE); // WARNING

	CASE_REMAP_PRE(CMP, LT32F, LT);
	CASE_REMAP_PRE(CMP, LT32I, LT);
	CASE_REMAP_PRE(CMP, LT32U, LT); // WARNING

	CASE_REMAP_PRE(CMP, NE32F, NE);
	CASE_REMAP_PRE(CMP, NE32I, NE);

	// Logical
	CASE_REMAP_PRE(LOGICAL, AND32F, AND);
	CASE_REMAP_PRE(LOGICAL, AND32I, AND);

	CASE_REMAP_PRE(LOGICAL, IOR32F, IOR);
	CASE_REMAP_PRE(LOGICAL, IOR32I, IOR);

	CASE_REMAP_PRE(LOGICAL, NOT32F, NOT);
	CASE_REMAP_PRE(LOGICAL, NOT32I, NOT);

	// Stack-ops.
	CASE_REMAP_PRE(STACK, DROP32, DROP);

	// Variable Address

	// Variable Get
	CASE_REMAP_PRE(GET, LITERAL32F, LITERAL);
	CASE_REMAP_PRE(GET, LITERAL32I, LITERAL);

	CASE_REMAP_PRE(GET, MAPREGISTER_VAR32F, MAPREGISTER_VAR);
	CASE_REMAP_PRE(GET, MAPREGISTER_VAR32I, MAPREGISTER_VAR);

	CASE_REMAP_PRE(GET, REGISTER_VAR32F, REGISTER_VAR);
	CASE_REMAP_PRE(GET, REGISTER_VAR32I, REGISTER_VAR);

	CASE_REMAP_PRE(GET, WORLDREGISTER_VAR32F, WORLDREGISTER_VAR);
	CASE_REMAP_PRE(GET, WORLDREGISTER_VAR32I, WORLDREGISTER_VAR);

	// Variable Set
	CASE_REMAP_PRE(SET, MAPREGISTER_VAR32F, MAPREGISTER_VAR);
	CASE_REMAP_PRE(SET, MAPREGISTER_VAR32I, MAPREGISTER_VAR);

	CASE_REMAP_PRE(SET, REGISTER_VAR32F, REGISTER_VAR);
	CASE_REMAP_PRE(SET, REGISTER_VAR32I, REGISTER_VAR);

	CASE_REMAP_PRE(SET, WORLDREGISTER_VAR32F, WORLDREGISTER_VAR);
	CASE_REMAP_PRE(SET, WORLDREGISTER_VAR32I, WORLDREGISTER_VAR);

	// Miscellaneous
	CASE_REMAP_PRE(MISC, DELAY32,     DELAY);
	CASE_REMAP_PRE(MISC, DELAY32_IMM, DELAY_IMM);

	CASE_REMAP_PRE(MISC, NEGATE32F, NEGATE);
	CASE_REMAP_PRE(MISC, NEGATE32I, NEGATE);

	CASE_REMAP_PRE(MISC, RANDOM32F,     RANDOM);
	CASE_REMAP_PRE(MISC, RANDOM32F_IMM, RANDOM_IMM);
	CASE_REMAP_PRE(MISC, RANDOM32I,     RANDOM);
	CASE_REMAP_PRE(MISC, RANDOM32I_IMM, RANDOM_IMM);
	CASE_REMAP_PRE(MISC, RANDOM32U,     RANDOM);     // WARNING
	CASE_REMAP_PRE(MISC, RANDOM32U_IMM, RANDOM_IMM); // WARNING

	// ACS
	CASE_MAP_ACS(LINESPEC1);
	CASE_MAP_ACS(LINESPEC1_IMM);
	CASE_MAP_ACS(LINESPEC2);
	CASE_MAP_ACS(LINESPEC2_IMM);
	CASE_MAP_ACS(LINESPEC3);
	CASE_MAP_ACS(LINESPEC3_IMM);
	CASE_MAP_ACS(LINESPEC4);
	CASE_MAP_ACS(LINESPEC4_IMM);
	CASE_MAP_ACS(LINESPEC5);
	CASE_MAP_ACS(LINESPEC5_IMM);
	CASE_MAP_ACS(SCRIPT_RESTART);
	CASE_MAP_ACS(SCRIPT_SUSPEND);
	CASE_MAP_ACS(SCRIPT_TERMINATE);
	CASE_MAP_ACS(THINGCOUNT);
	CASE_MAP_ACS(THINGCOUNT_IMM);
	CASE_MAP_ACS(TIMER);

	// ACS Common Extensions

	// ACS Extensions

	// ACS Printing
	CASE_MAP_ACSP(CHARACTER);
	CASE_MAP_ACSP(END);
	CASE_MAP_ACSP(END_BOLD);
	CASE_REMAP_ACSP(NUM_DEC32I, NUM_DEC);
	CASE_REMAP_ACSP(NUM_DEC32U, NUM_DEC); // WARNING
	CASE_MAP_ACSP(START);
	CASE_MAP_ACSP(STRING);



	// Translations


	// Conversion

	case OCODE_CONVERT_32F_32I:
		args.push_back(fracbits);
		PUSH_TOKEN(BCODE_GET_LITERAL);
		PUSH_TOKEN(BCODE_BITWISE_SHIFTR);
		break;

	case OCODE_CONVERT_32I_32F:
		args.push_back(fracbits);
		PUSH_TOKEN(BCODE_GET_LITERAL);
		PUSH_TOKEN(BCODE_BITWISE_SHIFTL);
		break;

	case OCODE_NONE:
	default:
		throw SourceException("unknown OCODE: " + (std::string)make_string(object.getCode()), position, "BinaryTokenPPACS");
	}
}
void BinaryTokenACS::make_tokens(ObjectVector const & objects, std::vector<BinaryTokenACS> * instructions)
{
	for (bigsint index(0); index < objects.size(); ++index)
		make_tokens(objects[index], instructions);
}



