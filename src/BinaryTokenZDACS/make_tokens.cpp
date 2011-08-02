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

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"



void BinaryTokenZDACS::make_tokens(ObjectToken const & object, std::vector<BinaryTokenZDACS> * instructions)
{
	static std::vector<std::string> const nolabels;

	#define PUSH_ARGS2(START,STOP)\
		for (uintptr_t i(START); i < STOP; ++i)\
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
	case ObjectToken::OCODE_##CODE:\
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
	CASE_DIRECTMAP(ASSIGNMAPVAR);
	CASE_DIRECTMAP(ASSIGNSCRIPTVAR);
	CASE_DIRECTMAP(ASSIGNWORLDVAR);
	CASE_DIRECTMAP(BEGINPRINT);
	CASE_DIRECTMAP(BRANCH);
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

	case ObjectToken::OCODE_ADDSTACK_IMM:
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

	case ObjectToken::OCODE_ASSIGNGLOBALARRAY2:
		bcode = BCODE_ASSIGNGLOBALARRAY;
		goto assignarray_case;

	case ObjectToken::OCODE_ASSIGNMAPARRAY2:
		bcode = BCODE_ASSIGNMAPARRAY;
		goto assignarray_case;

	case ObjectToken::OCODE_ASSIGNSTACKARRAY2:
		PUSH_TOKEN_ARGS2(BCODE_PUSHNUMBER, 0, 1);
		PUSH_TOKEN_ARGS2(BCODE_PUSHNUMBER, 1, 2);
		PUSH_TOKEN(BCODE_ADD);
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

	case ObjectToken::OCODE_ASSIGNSTACKVAR:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		PUSH_TOKEN(BCODE_SWAP);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_ASSIGNGLOBALARRAY);
		break;

	case ObjectToken::OCODE_ASSIGNSTATICARRAY2:
		PUSH_TOKEN_ARGS2(BCODE_PUSHNUMBER, 0, 1);
		PUSH_TOKEN_ARGS2(BCODE_PUSHNUMBER, 1, 2);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::static_offset);
		PUSH_TOKEN(BCODE_PUSHNUMBER);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		PUSH_TOKEN(BCODE_SWAP);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_ASSIGNGLOBALARRAY);
		break;

	case ObjectToken::OCODE_ASSIGNSTATICVAR:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::static_offset);
		PUSH_TOKEN(BCODE_PUSHNUMBER);
		PUSH_TOKEN(BCODE_ADD);
		PUSH_TOKEN(BCODE_SWAP);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_ASSIGNGLOBALARRAY);
		break;

	case ObjectToken::OCODE_ASSIGNWORLDARRAY2:
		bcode = BCODE_ASSIGNWORLDARRAY;
		goto assignarray_case;

	pusharray_case:
		PUSH_TOKEN_ARGS2(BCODE_PUSHNUMBER, 1, 2);
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		PUSH_TOKEN_ARGS2(bcode, 0, 1);
		break;

	case ObjectToken::OCODE_PUSHGLOBALARRAY2:
		bcode = BCODE_PUSHGLOBALARRAY;
		goto pusharray_case;

	case ObjectToken::OCODE_PUSHMAPARRAY2:
		bcode = BCODE_PUSHMAPARRAY;
		goto pusharray_case;

	case ObjectToken::OCODE_PUSHSTACKADDRESS:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		break;

	case ObjectToken::OCODE_PUSHSTACKARRAY2:
		PUSH_TOKEN_ARGS2(BCODE_PUSHNUMBER, 0, 1);
		PUSH_TOKEN_ARGS2(BCODE_PUSHNUMBER, 1, 2);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHGLOBALARRAY);
		break;

	case ObjectToken::OCODE_PUSHSTACKVAR:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHGLOBALARRAY);
		break;

	case ObjectToken::OCODE_PUSHSTATICARRAY2:
		PUSH_TOKEN_ARGS2(BCODE_PUSHNUMBER, 0, 1);
		PUSH_TOKEN_ARGS2(BCODE_PUSHNUMBER, 1, 2);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::static_offset);
		PUSH_TOKEN(BCODE_PUSHNUMBER);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(1, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHWORLDVAR);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHGLOBALARRAY);
		break;

	case ObjectToken::OCODE_PUSHSTATICVAR:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::static_offset);
		PUSH_TOKEN(BCODE_PUSHNUMBER);
		PUSH_TOKEN(BCODE_ADD);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_PUSHGLOBALARRAY);
		break;

	case ObjectToken::OCODE_PUSHWORLDARRAY2:
		bcode = BCODE_PUSHWORLDARRAY;
		goto pusharray_case;

	case ObjectToken::OCODE_SUBSTACK_IMM:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_SUBWORLDVAR);
		break;

	case ObjectToken::OCODE_NONE:
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
	for (ObjectExpression::int_t index(0); index < objects.size(); ++index)
		make_tokens(objects[index], instructions);
}


