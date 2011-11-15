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



void BinaryTokenPPACS::make_tokens(ObjectToken const & object, std::vector<BinaryTokenPPACS> * instructions)
{
	static std::vector<std::string> const nolabels;

	#define PUSH_ARGS2(START,STOP)\
		for (int i(START); i < STOP; ++i)\
			args.push_back(object.getArg(i))

	#define PUSH_ARGS1(STOP)\
		PUSH_ARGS2(0, STOP)

	#define PUSH_TOKEN(BCODE)\
		instructions->push_back(BinaryTokenPPACS(\
			BCODE,\
			position,\
			*labels,\
			args\
		));\
		args.clear();\
		labels = &nolabels;

	#define PUSH_TOKEN_ARGS1(BCODE,STOP)\
		PUSH_ARGS1(STOP);\
		PUSH_TOKEN(BCODE)

	#define PUSH_TOKEN_ARGS2(BCODE,START,STOP)\
		PUSH_ARGS2(START, STOP);\
		PUSH_TOKEN(BCODE)

	#define CASE_DIRECTMAP(CODE)\
	case OCODE_##CODE:\
		PUSH_TOKEN_ARGS1(BCODE_##CODE, _arg_counts[BCODE_##CODE]);\
		break

	#define CASE_DIRECTMAP_ACS(CODE)\
	case OCODE_ACS_##CODE:\
		PUSH_TOKEN_ARGS1(BCODE_##CODE, _arg_counts[BCODE_##CODE]);\
		break

	#define CASE_DIRECTMAP_PPACS(CODE)\
	case OCODE_PPACS_##CODE:\
		PUSH_TOKEN_ARGS1(BCODE_##CODE, _arg_counts[BCODE_##CODE]);\
		break

	#define CASE_REMAP(OCODE, BCODE)\
	case OCODE_##OCODE:\
		PUSH_TOKEN_ARGS1(BCODE_##BCODE, _arg_counts[BCODE_##BCODE]);\
		break

	std::vector<ObjectExpression::Pointer> args;

	SourcePosition const & position(object.getPosition());

	std::vector<std::string> const * labels(&object.getLabels());

	switch (object.getCode())
	{
	CASE_DIRECTMAP(ADD);
	CASE_DIRECTMAP(ADDWORLDVAR);
	CASE_DIRECTMAP(AND);
	CASE_DIRECTMAP(ASSIGNMAPVAR);
	CASE_DIRECTMAP(ASSIGNSCRIPTVAR);
	CASE_DIRECTMAP(ASSIGNWORLDVAR);
	CASE_DIRECTMAP(BEGINPRINT);
	CASE_DIRECTMAP(BRANCH_IMM);
	CASE_DIRECTMAP(BRANCHCASE);
	CASE_DIRECTMAP(BRANCHNOTZERO);
	CASE_DIRECTMAP(BRANCHZERO);
	CASE_DIRECTMAP(CMPEQ);
	CASE_DIRECTMAP(CMPGE);
	CASE_DIRECTMAP(CMPGT);
	CASE_DIRECTMAP(CMPLE);
	CASE_DIRECTMAP(CMPLT);
	CASE_DIRECTMAP(CMPNE);
	CASE_DIRECTMAP(DELAY);
	CASE_DIRECTMAP(DELAY_IMM);
	CASE_DIRECTMAP(DIV);
	CASE_DIRECTMAP(DROP);
	CASE_DIRECTMAP(ENDPRINT);
	CASE_DIRECTMAP(GET_POINTER_VAR);
	CASE_DIRECTMAP(IOR);
	CASE_DIRECTMAP(LOGICALAND);
	CASE_DIRECTMAP(LOGICALIOR);
	CASE_DIRECTMAP(LOGICALNOT);
	CASE_DIRECTMAP(LSPEC1);
	CASE_DIRECTMAP(LSPEC1_IMM);
	CASE_DIRECTMAP(LSPEC2);
	CASE_DIRECTMAP(LSPEC2_IMM);
	CASE_DIRECTMAP(LSPEC3);
	CASE_DIRECTMAP(LSPEC3_IMM);
	CASE_DIRECTMAP(LSPEC4);
	CASE_DIRECTMAP(LSPEC4_IMM);
	CASE_DIRECTMAP(LSPEC5);
	CASE_DIRECTMAP(LSPEC5_IMM);
	CASE_DIRECTMAP(MUL);
	CASE_DIRECTMAP(MOD);
	CASE_DIRECTMAP(NEGATE);
	CASE_DIRECTMAP(NOP);
	CASE_DIRECTMAP(PRINTCHARACTER);
	CASE_DIRECTMAP(PRINTNUMBER);
	CASE_DIRECTMAP(PRINTSTRING);
	CASE_DIRECTMAP(PUSHMAPVAR);
	CASE_DIRECTMAP(PUSHNUMBER);
	CASE_DIRECTMAP(PUSHSCRIPTVAR);
	CASE_DIRECTMAP(PUSHWORLDVAR);
	CASE_DIRECTMAP(RANDOM);
	CASE_DIRECTMAP(RANDOM_IMM);
	CASE_DIRECTMAP(RESTART);
	CASE_DIRECTMAP(SET_POINTER_VAR);
	CASE_DIRECTMAP(SHIFTL);
	CASE_DIRECTMAP(SHIFTR);
	CASE_DIRECTMAP(SUB);
	CASE_DIRECTMAP(SUBWORLDVAR);
	CASE_DIRECTMAP(SUSPEND);
	CASE_DIRECTMAP(TERMINATE);
	CASE_DIRECTMAP(XOR);

	CASE_DIRECTMAP(BRANCH);

	CASE_DIRECTMAP_ACS(THING_COUNT);
	CASE_DIRECTMAP_ACS(THING_COUNT_IMM);
	CASE_DIRECTMAP_ACS(TIMER);

	CASE_DIRECTMAP_PPACS(GET_ACTIVATOR_MID);
	CASE_DIRECTMAP_PPACS(GET_MOBJ_VAR);
	CASE_DIRECTMAP_PPACS(SET_ACTIVATOR_MID);
	CASE_DIRECTMAP_PPACS(SET_MOBJ_VAR);

	CASE_REMAP(ASSIGNSTACKVAR,  SET_AUTO_VAR);
	CASE_REMAP(ASSIGNSTATICVAR, SET_STATIC_VAR);
	CASE_REMAP(DIVFIXED, DIV_FIXED);
	CASE_REMAP(MULFIXED, MUL_FIXED);
	CASE_REMAP(PRINTFIXED, PRINT_FIXED);
	CASE_REMAP(PUSHSTACKADDRESS, GET_AUTO_ADDRESS);
	CASE_REMAP(PUSHSTACKVAR,     GET_AUTO_VAR);
	CASE_REMAP(PUSHSTATICVAR,    GET_STATIC_VAR);

	case OCODE_ADDSTACK_IMM:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		PUSH_TOKEN(BCODE_ADD_AUTO_ADDRESS);
		break;

	case OCODE_SUBSTACK_IMM:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		PUSH_TOKEN(BCODE_SUB_AUTO_ADDRESS);
		break;

	case OCODE_ASSIGNPOINTER:
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN_ARGS1(BCODE_SET_POINTER_VAR, 1);
		break;

	case OCODE_PUSHPOINTER:
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN_ARGS1(BCODE_GET_POINTER_VAR, 1);
		break;

	case OCODE_LOGICALXOR:
	case OCODE_NONE:
	default:
		throw SourceException("unknown OCODE: " + (std::string)make_string(object.getCode()), position, "BinaryTokenPPACS");
	}

	#undef CASE_REMAP

	#undef CASE_DIRECTMAP_PPACS
	#undef CASE_DIRECTMAP_ACS
	#undef CASE_DIRECTMAP

	#undef PUSH_TOKEN_ARGS2
	#undef PUSH_TOKEN_ARGS1

	#undef PUSH_TOKEN
	#undef PUSH_ARGS1
	#undef PUSH_ARGS2
}
void BinaryTokenPPACS::make_tokens(ObjectVector const & objects, std::vector<BinaryTokenPPACS> * instructions)
{
	for (bigsint index(0); index < objects.size(); ++index)
		make_tokens(objects[index], instructions);
}



