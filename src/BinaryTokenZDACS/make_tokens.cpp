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



void BinaryTokenZDACS::make_tokens(ObjectToken const & object, std::vector<BinaryTokenZDACS> * instructions)
{
	static std::vector<std::string> const nolabels;

	#define PUSH_ARGS2(START,STOP)\
		for (int i(START); i < STOP; ++i)\
			args.push_back(object.getArg(i))

	#define PUSH_ARGS1(STOP)\
		PUSH_ARGS2(0, STOP)

	#define PUSH_TOKEN(BCODE)\
		instructions->push_back(BinaryTokenZDACS(\
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

	#define CASE_DIRECTMAP_ZDACS(CODE)\
	case OCODE_ZDACS_##CODE:\
		PUSH_TOKEN_ARGS1(BCODE_##CODE, _arg_counts[BCODE_##CODE]);\
		break

	std::vector<ObjectExpression::Pointer> args;

	BinaryCode bcode;

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
	CASE_DIRECTMAP(BRANCH);
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
	CASE_DIRECTMAP(SHIFTL);
	CASE_DIRECTMAP(SHIFTR);
	CASE_DIRECTMAP(SUB);
	CASE_DIRECTMAP(SUBWORLDVAR);
	CASE_DIRECTMAP(SUSPEND);
	CASE_DIRECTMAP(TERMINATE);
	CASE_DIRECTMAP(XOR);

	CASE_DIRECTMAP(ASSIGNGLOBALARRAY);
	CASE_DIRECTMAP(ASSIGNGLOBALVAR);
	CASE_DIRECTMAP(ASSIGNMAPARRAY);
	CASE_DIRECTMAP(ASSIGNWORLDARRAY);
	CASE_DIRECTMAP(CALLZDACS);
	CASE_DIRECTMAP(CALLZDACSDISCARD);
	CASE_DIRECTMAP(CALLZDFUNC);
	CASE_DIRECTMAP(DIVFIXED);
	CASE_DIRECTMAP(DUP);
	CASE_DIRECTMAP(ENDLOG);
	CASE_DIRECTMAP(LSPEC5RESULT);
	CASE_DIRECTMAP(MULFIXED);
	CASE_DIRECTMAP(NOT);
	CASE_DIRECTMAP(PRINTFIXED);
	CASE_DIRECTMAP(PRINTHEX);
	CASE_DIRECTMAP(PUSHGLOBALARRAY);
	CASE_DIRECTMAP(PUSHGLOBALVAR);
	CASE_DIRECTMAP(PUSHMAPARRAY);
	CASE_DIRECTMAP(PUSHWORLDARRAY);
	CASE_DIRECTMAP(RETURNZDACS);
	CASE_DIRECTMAP(RETURNZDACSVOID);
	CASE_DIRECTMAP(SETRESULTVALUE);
	CASE_DIRECTMAP(STRLEN);
	CASE_DIRECTMAP(SWAP);
	CASE_DIRECTMAP(TRIG_COS);
	CASE_DIRECTMAP(TRIG_SIN);

	CASE_DIRECTMAP_ACS(THING_COUNT);
	CASE_DIRECTMAP_ACS(THING_COUNT_IMM);
	CASE_DIRECTMAP_ACS(TIMER);

	CASE_DIRECTMAP_ZDACS(ACTIVATOR_TID);
	CASE_DIRECTMAP_ZDACS(GET_ACTOR_ANGLE);
	CASE_DIRECTMAP_ZDACS(GET_ACTOR_PITCH);
	CASE_DIRECTMAP_ZDACS(GET_ACTOR_PROPERTY);
	CASE_DIRECTMAP_ZDACS(GET_ACTOR_X);
	CASE_DIRECTMAP_ZDACS(GET_ACTOR_Y);
	CASE_DIRECTMAP_ZDACS(GET_ACTOR_Z);
	CASE_DIRECTMAP_ZDACS(GET_AMMO_CAPACITY);
	CASE_DIRECTMAP_ZDACS(INVENTORY_CHECK);
	CASE_DIRECTMAP_ZDACS(INVENTORY_CHECK_IMM);
	CASE_DIRECTMAP_ZDACS(INVENTORY_GIVE);
	CASE_DIRECTMAP_ZDACS(INVENTORY_GIVE_IMM);
	CASE_DIRECTMAP_ZDACS(INVENTORY_TAKE);
	CASE_DIRECTMAP_ZDACS(INVENTORY_TAKE_IMM);
	CASE_DIRECTMAP_ZDACS(PLAYER_NUMBER);
	CASE_DIRECTMAP_ZDACS(SET_ACTOR_ANGLE);
	CASE_DIRECTMAP_ZDACS(SET_ACTOR_PITCH);
	CASE_DIRECTMAP_ZDACS(SET_ACTOR_POSITION);
	CASE_DIRECTMAP_ZDACS(SET_ACTOR_PROPERTY);
	CASE_DIRECTMAP_ZDACS(SET_AMMO_CAPACITY);
	CASE_DIRECTMAP_ZDACS(SPAWN);
	CASE_DIRECTMAP_ZDACS(SPAWN_IMM);
	CASE_DIRECTMAP_ZDACS(SPAWN_PROJECTILE);

	case OCODE_ADDSTACK_IMM:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_ADDWORLDVAR);
		break;

	assignarray_case:
		PUSH_TOKEN_ARGS2(BCODE_PUSHNUMBER, 1, 2);
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		PUSH_TOKEN(BCODE_SWAP);
		PUSH_TOKEN_ARGS2(bcode, 0, 1);
		break;

	case OCODE_ASSIGNGLOBALARRAY2:
		bcode = BCODE_ASSIGNGLOBALARRAY;
		goto assignarray_case;

	case OCODE_ASSIGNMAPARRAY2:
		bcode = BCODE_ASSIGNMAPARRAY;
		goto assignarray_case;

	case OCODE_ASSIGNPOINTER:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		PUSH_TOKEN(BCODE_SWAP);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_ASSIGNGLOBALARRAY);
		break;

	case OCODE_ASSIGNSTACKARRAY2:
		args.push_back(ObjectExpression::create_binary_add(object.getArg(0), object.getArg(1), SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHNUMBER);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		PUSH_TOKEN(BCODE_SWAP);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_ASSIGNGLOBALARRAY);
		break;

	case OCODE_ASSIGNSTACKVAR:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		PUSH_TOKEN(BCODE_SWAP);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_ASSIGNGLOBALARRAY);
		break;

	case OCODE_ASSIGNSTATICARRAY2:
		args.push_back(ObjectExpression::create_binary_add(ObjectExpression::create_binary_add(object.getArg(0), object.getArg(1), SourcePosition::none), ObjectExpression::static_offset, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHNUMBER);
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		PUSH_TOKEN(BCODE_SWAP);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_ASSIGNGLOBALARRAY);
		break;

	case OCODE_ASSIGNSTATICVAR:
		args.push_back(ObjectExpression::create_binary_add(object.getArg(0), ObjectExpression::static_offset, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHNUMBER);
		PUSH_TOKEN(BCODE_SWAP);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_ASSIGNGLOBALARRAY);
		break;

	case OCODE_ASSIGNWORLDARRAY2:
		bcode = BCODE_ASSIGNWORLDARRAY;
		goto assignarray_case;

	case OCODE_BRANCHTABLE:
		args = object.getArgs();
		if (args.size() % 2) throw SourceException("uneven OCODE_BRANCHTABLE", position, "BinaryTokenZDACS");
		PUSH_TOKEN(BCODE_BRANCHTABLE);
		break;

	pusharray_case:
		PUSH_TOKEN_ARGS2(BCODE_PUSHNUMBER, 1, 2);
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		PUSH_TOKEN_ARGS2(bcode, 0, 1);
		break;

	case OCODE_PUSHGLOBALARRAY2:
		bcode = BCODE_PUSHGLOBALARRAY;
		goto pusharray_case;

	case OCODE_PUSHMAPARRAY2:
		bcode = BCODE_PUSHMAPARRAY;
		goto pusharray_case;

	case OCODE_PUSHPOINTER:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHGLOBALARRAY);
		break;

	case OCODE_PUSHSTACKADDRESS:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		break;

	case OCODE_PUSHSTACKARRAY2:
		args.push_back(ObjectExpression::create_binary_add(object.getArg(0), object.getArg(1), SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHNUMBER);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHGLOBALARRAY);
		break;

	case OCODE_PUSHSTACKVAR:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHGLOBALARRAY);
		break;

	case OCODE_PUSHSTATICARRAY2:
		args.push_back(ObjectExpression::create_binary_add(ObjectExpression::create_binary_add(object.getArg(0), object.getArg(1), SourcePosition::none), ObjectExpression::static_offset, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHNUMBER);
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHGLOBALARRAY);
		break;

	case OCODE_PUSHSTATICVAR:
		args.push_back(ObjectExpression::create_binary_add(object.getArg(0), ObjectExpression::static_offset, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHNUMBER);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHGLOBALARRAY);
		break;

	case OCODE_PUSHWORLDARRAY2:
		bcode = BCODE_PUSHWORLDARRAY;
		goto pusharray_case;

	case OCODE_SUBSTACK_IMM:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_SUBWORLDVAR);
		break;

	case OCODE_LOGICALXOR:
	case OCODE_NONE:
		throw SourceException("unknown OCODE", position, "BinaryTokenZDACS");
	}

	#undef CASE_DIRECTMAP

	#undef PUSH_TOKEN_ARGS2
	#undef PUSH_TOKEN_ARGS1

	#undef PUSH_TOKEN
	#undef PUSH_ARGS1
	#undef PUSH_ARGS2
}
void BinaryTokenZDACS::make_tokens(ObjectVector const & objects, std::vector<BinaryTokenZDACS> * instructions)
{
	for (bigsint index(0); index < objects.size(); ++index)
		make_tokens(objects[index], instructions);
}



