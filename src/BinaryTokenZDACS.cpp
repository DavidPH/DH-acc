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

/* BinaryTokenZDACS.cpp
**
** BinaryTokenZDACS methods.
*/

#include "BinaryTokenZDACS.hpp"

#include "ObjectToken.hpp"
#include "ObjectVector.hpp"
#include "ost_type.hpp"
#include "SourceException.hpp"



uintptr_t BinaryTokenZDACS::_arg_counts[BCODE_NONE];



BinaryTokenZDACS::BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<ObjectExpression::Pointer> const & args) : _args(args), _code(code), _labels(labels), _position(position)
{

}

void BinaryTokenZDACS::addLabel(std::string const & label)
{
	_labels.push_back(label);
}

void BinaryTokenZDACS::init()
{
	#define DO_INIT(NAME,ARGC)\
	_arg_counts[BCODE_##NAME] = ARGC

	// BinaryTokenACS
	DO_INIT(ADD,             0);
	DO_INIT(ADDWORLDVAR,     1);
	DO_INIT(ASSIGNMAPVAR,    1);
	DO_INIT(ASSIGNSCRIPTVAR, 1);
	DO_INIT(ASSIGNWORLDVAR,  1);
	DO_INIT(BEGINPRINT,      0);
	DO_INIT(BRANCH,          1);
	DO_INIT(BRANCHNOTZERO,   1);
	DO_INIT(BRANCHZERO,      1);
	DO_INIT(DELAY,           0);
	DO_INIT(DELAY_IMM,       1);
	DO_INIT(DIV,             0);
	DO_INIT(DROP,            0);
	DO_INIT(ENDPRINT,        0);
	DO_INIT(LSPEC1,          1);
	DO_INIT(LSPEC1_IMM,      2);
	DO_INIT(LSPEC2,          1);
	DO_INIT(LSPEC2_IMM,      3);
	DO_INIT(LSPEC3,          1);
	DO_INIT(LSPEC3_IMM,      4);
	DO_INIT(LSPEC4,          1);
	DO_INIT(LSPEC4_IMM,      5);
	DO_INIT(LSPEC5,          1);
	DO_INIT(LSPEC5_IMM,      6);
	DO_INIT(MUL,             0);
	DO_INIT(MOD,             0);
	DO_INIT(NOP,             0);
	DO_INIT(PRINTCHARACTER,  0);
	DO_INIT(PRINTNUMBER,     0);
	DO_INIT(PRINTSTRING,     0);
	DO_INIT(PUSHMAPVAR,      1);
	DO_INIT(PUSHNUMBER,      1);
	DO_INIT(PUSHSCRIPTVAR,   1);
	DO_INIT(PUSHWORLDVAR,    1);
	DO_INIT(RANDOM,          0);
	DO_INIT(RANDOM_IMM,      2);
	DO_INIT(RESTART,         0);
	DO_INIT(SHIFTL,          0);
	DO_INIT(SHIFTR,          0);
	DO_INIT(SUB,             0);
	DO_INIT(SUBWORLDVAR,     1);
	DO_INIT(SUSPEND,         0);
	DO_INIT(TERMINATE,       0);

	// BinaryTokenZDACS
	DO_INIT(ASSIGNGLOBALARRAY, 1);
	DO_INIT(ASSIGNGLOBALVAR,   1);
	DO_INIT(ASSIGNMAPARRAY,    1);
	DO_INIT(ASSIGNWORLDARRAY,  1);
	DO_INIT(CALLZDACS,         1);
	DO_INIT(CALLZDACSDISCARD,  1);
	DO_INIT(CALLZDFUNC,        2);
	DO_INIT(DIVFIXED,          0);
	DO_INIT(DUP,               0);
	DO_INIT(ENDLOG,            0);
	DO_INIT(LSPEC5RESULT,      1);
	DO_INIT(MULFIXED,          0);
	DO_INIT(PRINTFIXED,        0);
	DO_INIT(PUSHGLOBALARRAY,   1);
	DO_INIT(PUSHGLOBALVAR,     1);
	DO_INIT(PUSHMAPARRAY,      1);
	DO_INIT(PUSHWORLDARRAY,    1);
	DO_INIT(RETURNZDACS,       0);
	DO_INIT(RETURNZDACSVOID,   0);
	DO_INIT(SETRESULTVALUE,    0);
	DO_INIT(STRLEN,            0);
	DO_INIT(SWAP,              0);

	#undef DO_INIT
}

void BinaryTokenZDACS::make_tokens(ObjectVector const & objects, std::vector<BinaryTokenZDACS> * const instructions)
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

	for (ObjectExpression::int_t index(0); index < objects.size(); ++index)
	{
	ObjectToken const & object(objects[index]);

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

	case ObjectToken::OCODE_PUSHWORLDARRAY2:
		bcode = BCODE_PUSHWORLDARRAY;
		goto pusharray_case;

	case ObjectToken::OCODE_SUBSTACK_IMM:
		PUSH_TOKEN_ARGS1(BCODE_PUSHNUMBER, 1);
		args.push_back(ObjectExpression::create_value_int(0, SourcePosition::none));
		PUSH_TOKEN(BCODE_SUBWORLDVAR);
		break;

	case ObjectToken::OCODE_NONE:
		throw SourceException("unknown OCODE", objects[index].getPosition(), "BinaryTokenZDACS");
	}
	}

	#undef CASE_DIRECTMAP

	#undef PUSH_TOKEN_ARGS2
	#undef PUSH_TOKEN_ARGS1

	#undef PUSH_TOKEN
	#undef PUSH_ARGS1
	#undef PUSH_ARGS2
}

void BinaryTokenZDACS::write(std::ostream * const out) const
{
	write_32(out, _code);

	for (uintptr_t i(0); i < _arg_counts[_code]; ++i)
	{
		if (i < _args.size())
			write_32(out, *_args[i]);
		else
			write_32(out, 0);
	}
}

void BinaryTokenZDACS::write_8(std::ostream * const out, uint8_t const i)
{
	out->put((i >> 0) & 0xFF);
}
void BinaryTokenZDACS::write_16(std::ostream * const out, uint16_t const i)
{
	out->put((i >> 0) & 0xFF);
	out->put((i >> 8) & 0xFF);
}
void BinaryTokenZDACS::write_32(std::ostream * const out, ObjectExpression const & expr)
{
	switch (expr.getType())
	{
	case ObjectExpression::ET_FLOAT:
		write_32(out, (int32_t)(expr.resolveFloat() * 65536.0));
		break;

	case ObjectExpression::ET_INT:
		write_32(out, (int32_t)expr.resolveInt());
		break;
	}
}
void BinaryTokenZDACS::write_32(std::ostream * const out, uint32_t const i)
{
	out->put((i >>  0) & 0xFF);
	out->put((i >>  8) & 0xFF);
	out->put((i >> 16) & 0xFF);
	out->put((i >> 24) & 0xFF);
}

void BinaryTokenZDACS::write_acsfunc(std::ostream * const out, ObjectExpression::ACSFunc const & f)
{
	switch (output_type)
	{
	case OUTPUT_ACSE:
		write_8 (out, (int8_t)f.argCount);
		write_8 (out, (int8_t)f.varCount);
		write_8 (out, (int8_t)f.retCount);
		write_8 (out, 0);
		write_32(out, *ObjectExpression::get_symbol(f.label, SourcePosition::none));
		break;

	default:
		throw SourceException("unknown output type for acsfunc", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_all(std::ostream * const out, std::vector<BinaryTokenZDACS> const & instructions)
{
	int32_t directoryOffset(8);
	ObjectExpression::set_address_count(8);

	for (uintptr_t index(0); index < instructions.size(); ++index)
	{
		for (uintptr_t i(0); i < instructions[index]._labels.size(); ++i)
			ObjectExpression::add_label(instructions[index]._labels[i]);

		int32_t size(_arg_counts[instructions[index]._code]*4 + 4);

		directoryOffset += size;
		ObjectExpression::add_address_count(size);
	}

	switch (output_type)
	{
	case OUTPUT_ACS0:
	{
		int32_t const scriptCount(ObjectExpression::get_script_count());
		int32_t const stringCount(ObjectExpression::get_string_count());

		// 0
		*out << 'A' << 'C' << 'S' << '\0';
		write_32(out, directoryOffset);

		// 8
		for (uintptr_t index(0); index < instructions.size(); ++index)
			instructions[index].write(out);

		// directoryOffset
		write_32(out, scriptCount);

		// directoryOffset+4
		for (int32_t index(0); index < scriptCount; ++index)
			write_script(out, ObjectExpression::get_script(index));

		// directoryOffset+4+(scriptCount*12)
		write_32(out, stringCount);

		// directoryOffset+4+(scriptCount*12)+4
		for (int32_t index(0); index < stringCount; ++index)
			write_32(out, directoryOffset + 4 + (scriptCount*12) + 4 + (stringCount*4) + ObjectExpression::get_string(index).offset);

		// directoryOffset+4+(scriptCount*12)+4+(stringCount*4)
		for (int32_t index(0); index < stringCount; ++index)
			write_string(out, ObjectExpression::get_string(index).string);
	}
		break;

	case OUTPUT_ACSE:
	{
		ObjectExpression::int_t const acsfuncCount(ObjectExpression::get_acsfunc_count());
		ObjectExpression::int_t const registerarrayMapCount(ObjectExpression::get_registerarray_map_count());
		int32_t const scriptCount(ObjectExpression::get_script_count());
		int32_t const stringCount(ObjectExpression::get_string_count());

		*out << 'A' << 'C' << 'S' << 'E';
		write_32(out, directoryOffset);

		for (uintptr_t index(0); index < instructions.size(); ++index)
			instructions[index].write(out);

		if (acsfuncCount)
		{
			*out << 'F' << 'U' << 'N' << 'C';
			write_32(out, acsfuncCount*8);

			for (ObjectExpression::int_t index(0); index < acsfuncCount; ++index)
				write_acsfunc(out, ObjectExpression::get_acsfunc(index));
		}

		if (registerarrayMapCount)
		{
			*out << 'A' << 'R' << 'A' << 'Y';
			write_32(out, registerarrayMapCount*8);

			for (ObjectExpression::int_t index(0); index < registerarrayMapCount; ++index)
				write_registerarray(out, ObjectExpression::get_registerarray_map(index));
		}

		if (scriptCount)
		{
			*out << 'S' << 'P' << 'T' << 'R';
			write_32(out, scriptCount*12);

			for (int32_t index(0); index < scriptCount; ++index)
				write_script(out, ObjectExpression::get_script(index));

			*out << 'S' << 'F' << 'L' << 'G';
			write_32(out, scriptCount*4);

			for (int32_t index(0); index < scriptCount; ++index)
				write_script_flags(out, ObjectExpression::get_script(index));

			*out << 'S' << 'V' << 'C' << 'T';
			write_32(out, scriptCount*4);

			for (int32_t index(0); index < scriptCount; ++index)
				write_script_vars(out, ObjectExpression::get_script(index));
		}

		if (stringCount)
		{
			int32_t const stringLength(ObjectExpression::get_string_length());

			*out << 'S' << 'T' << 'R' << 'L';
			write_32(out, 12 + (stringCount*4) + stringLength);

			write_32(out, 0);
			write_32(out, stringCount);
			write_32(out, 0);

			for (int32_t index(0); index < stringCount; ++index)
				write_32(out, 12 + (stringCount*4) + ObjectExpression::get_string(index).offset);

			for (int32_t index(0); index < stringCount; ++index)
				write_string(out, ObjectExpression::get_string(index).string);
		}
	}
		break;

	default:
		throw SourceException("unknown output type", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_registerarray(std::ostream * const out, ObjectExpression::RegisterArray const & r)
{
	switch (output_type)
	{
	case OUTPUT_ACSE:
		write_32(out, r.number);
		write_32(out, r.size);
		break;

	default:
		throw SourceException("unknown output type for registerarray", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_script(std::ostream * const out, ObjectExpression::Script const & s)
{
	switch (output_type)
	{
	case OUTPUT_ACS0:
		write_32(out, (int32_t)(s.type * 1000) + (int32_t)s.number);
		write_32(out, *ObjectExpression::get_symbol(s.label, SourcePosition::none));
		write_32(out, s.args);
		break;

	case OUTPUT_ACSE:
		write_16(out, (int16_t)s.number);
		write_16(out, (int16_t)s.type);
		write_32(out, *ObjectExpression::get_symbol(s.label, SourcePosition::none));
		write_32(out, s.args);
		break;

	default:
		throw SourceException("unknown output type for script", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_script_flags(std::ostream * const out, ObjectExpression::Script const & s)
{
	switch (output_type)
	{
	case OUTPUT_ACSE:
		write_16(out, (int16_t)s.number);
		write_16(out, (int16_t)s.flags);
		break;

	default:
		throw SourceException("unknown output type for script_flags", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_script_vars(std::ostream * const out, ObjectExpression::Script const & s)
{
	switch (output_type)
	{
	case OUTPUT_ACSE:
		write_16(out, (int16_t)s.number);
		write_16(out, (int16_t)s.vars);
		break;

	default:
		throw SourceException("unknown output type for script_vars", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_string(std::ostream * const out, std::string const & s)
{
	*out << s;
}



