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
#include "ost_type.hpp"
#include "SourceException.hpp"



uintptr_t BinaryTokenZDACS::_arg_counts[BCODE_NONE];



BinaryTokenZDACS::BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<ObjectExpression> const & args) : _args(args), _code(code), _labels(labels), _position(position)
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
	DO_INIT(ASSIGNSCRIPTVAR, 1);
	DO_INIT(BEGINPRINT,      0);
	DO_INIT(DELAY,           0);
	DO_INIT(DELAY_IMM,       1);
	DO_INIT(DIV,             0);
	DO_INIT(DROP,            0);
	DO_INIT(ENDPRINT,        0);
	DO_INIT(GOTO,            1);
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
	DO_INIT(PUSHNUMBER,      1);
	DO_INIT(PUSHSCRIPTVAR,   1);
	DO_INIT(RESTART,         0);
	DO_INIT(SHIFTL,          0);
	DO_INIT(SHIFTR,          0);
	DO_INIT(SUB,             0);
	DO_INIT(SUSPEND,         0);
	DO_INIT(TERMINATE,       0);

	// BinaryTokenZDACS
	DO_INIT(ASSIGNGLOBALARRAY, 1);
	DO_INIT(DIVFIXED,          0);
	DO_INIT(DUP,               0);
	DO_INIT(ENDLOG,            0);
	DO_INIT(LSPEC5RESULT,      1);
	DO_INIT(MULFIXED,          0);
	DO_INIT(PRINTFIXED,        0);
	DO_INIT(PUSHGLOBALARRAY,   1);
	DO_INIT(SETRESULTVALUE,    0);

	#undef DO_INIT
}

void BinaryTokenZDACS::make_tokens(std::vector<ObjectToken> const & objects, std::vector<BinaryTokenZDACS> * const instructions)
{
	#define CASE_DIRECTMAP(NAME)\
	case ObjectToken::OCODE_##NAME:\
	{\
		std::vector<ObjectExpression> args;\
		for (uintptr_t i(0); i < _arg_counts[BCODE_##NAME]; ++i)\
			args.push_back(objects[index].getArg(i));\
		instructions->push_back(BinaryTokenZDACS(BCODE_##NAME, objects[index].getPosition(), objects[index].getLabels(), args));\
	}\
	break

	for (uintptr_t index(0); index < objects.size(); ++index) switch (objects[index].getCode())
	{
	CASE_DIRECTMAP(ADD);
	CASE_DIRECTMAP(ASSIGNSCRIPTVAR);
	CASE_DIRECTMAP(BEGINPRINT);
	CASE_DIRECTMAP(DELAY);
	CASE_DIRECTMAP(DELAY_IMM);
	CASE_DIRECTMAP(DIV);
	CASE_DIRECTMAP(DROP);
	CASE_DIRECTMAP(ENDPRINT);
	CASE_DIRECTMAP(GOTO);
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
	CASE_DIRECTMAP(PUSHNUMBER);
	CASE_DIRECTMAP(PUSHSCRIPTVAR);
	CASE_DIRECTMAP(RESTART);
	CASE_DIRECTMAP(SHIFTL);
	CASE_DIRECTMAP(SHIFTR);
	CASE_DIRECTMAP(SUB);
	CASE_DIRECTMAP(SUSPEND);
	CASE_DIRECTMAP(TERMINATE);

	CASE_DIRECTMAP(ASSIGNGLOBALARRAY);
	CASE_DIRECTMAP(DIVFIXED);
	CASE_DIRECTMAP(DUP);
	CASE_DIRECTMAP(ENDLOG);
	CASE_DIRECTMAP(LSPEC5RESULT);
	CASE_DIRECTMAP(MULFIXED);
	CASE_DIRECTMAP(PRINTFIXED);
	CASE_DIRECTMAP(PUSHGLOBALARRAY);
	CASE_DIRECTMAP(SETRESULTVALUE);

	case ObjectToken::OCODE_NONE:
		throw SourceException("unknown OCODE", objects[index].getPosition(), "BinaryTokenZDACS");
	}

	#undef CASE_DIRECTMAP
}

void BinaryTokenZDACS::write(std::ostream * const out) const
{
	write_32(out, _code);

	for (uintptr_t i(0); i < _arg_counts[_code]; ++i)
		write_32(out, i < _args.size() ? _args[i].resolveInt32() : 0);
}

void BinaryTokenZDACS::write_16(std::ostream * const out, uint16_t const i)
{
	out->put((i >> 0) & 0xFF);
	out->put((i >> 8) & 0xFF);
}
void BinaryTokenZDACS::write_32(std::ostream * const out, uint32_t const i)
{
	out->put((i >>  0) & 0xFF);
	out->put((i >>  8) & 0xFF);
	out->put((i >> 16) & 0xFF);
	out->put((i >> 24) & 0xFF);
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
		int32_t const scriptCount(ObjectExpression::get_script_count());
		int32_t const stringCount(ObjectExpression::get_string_count());

		*out << 'A' << 'C' << 'S' << 'E';
		write_32(out, directoryOffset);

		for (uintptr_t index(0); index < instructions.size(); ++index)
			instructions[index].write(out);

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

void BinaryTokenZDACS::write_script(std::ostream * const out, ObjectExpression::Script const & s)
{
	switch (output_type)
	{
	case OUTPUT_ACS0:
		write_32(out, (int32_t)(s.type * 1000) + (int32_t)s.number);
		write_32(out, ObjectExpression::get_symbol(s.label, SourcePosition::none).resolveInt32());
		write_32(out, s.args);
		break;

	case OUTPUT_ACSE:
		write_16(out, (int16_t)s.number);
		write_16(out, (int16_t)s.type);
		write_32(out, ObjectExpression::get_symbol(s.label, SourcePosition::none).resolveInt32());
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



