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
** Defines the BinaryTokenZDACS methods.
*/

#include "BinaryTokenZDACS.hpp"

#include "ost_type.hpp"
#include "SourceException.hpp"

#include <sstream>



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
	DO_INIT(AND,             0);
	DO_INIT(ASSIGNMAPVAR,    1);
	DO_INIT(ASSIGNSCRIPTVAR, 1);
	DO_INIT(ASSIGNWORLDVAR,  1);
	DO_INIT(BEGINPRINT,      0);
	DO_INIT(BRANCH,          1);
	DO_INIT(BRANCHCASE,      2);
	DO_INIT(BRANCHNOTZERO,   1);
	DO_INIT(BRANCHZERO,      1);
	DO_INIT(CMPEQ,           0);
	DO_INIT(CMPGE,           0);
	DO_INIT(CMPGT,           0);
	DO_INIT(CMPLE,           0);
	DO_INIT(CMPLT,           0);
	DO_INIT(CMPNE,           0);
	DO_INIT(DELAY,           0);
	DO_INIT(DELAY_IMM,       1);
	DO_INIT(DIV,             0);
	DO_INIT(DROP,            0);
	DO_INIT(ENDPRINT,        0);
	DO_INIT(IOR,             0);
	DO_INIT(LOGICALAND,      0);
	DO_INIT(LOGICALIOR,      0);
	DO_INIT(LOGICALNOT,      0);
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
	DO_INIT(NEGATE,          0);
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
	DO_INIT(XOR,             0);

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
	DO_INIT(NOT,               0);
	DO_INIT(PRINTFIXED,        0);
	DO_INIT(PRINTHEX,          0);
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

	std::ostringstream chunkout;

	bigsint const functionCount(ObjectExpression::get_function_count());
	bigsint const registerarrayMapCount(ObjectExpression::get_registerarray_map_count());
	int32_t const scriptCount(ObjectExpression::get_script_count());
	int32_t const stringCount(ObjectExpression::get_string_count());

	switch (output_type)
	{
	case OUTPUT_ACS0:
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

		break;

	case OUTPUT_ACSE:
		// Header
		*out << 'A' << 'C' << 'S' << 'E';
		write_32(out, directoryOffset);

		// Instructions
		for (uintptr_t index(0); index < instructions.size(); ++index)
			instructions[index].write(out);

		// ARAY - Map Array Declarations
		for (bigsint index(0); index < registerarrayMapCount; ++index)
			write_registerarray(&chunkout, ObjectExpression::get_registerarray_map(index));

		write_chunk(out, &chunkout, "ARAY");

		// FUNC - Functions
		for (bigsint index(0); index < functionCount; ++index)
			write_function(&chunkout, ObjectExpression::get_function(index));

		write_chunk(out, &chunkout, "FUNC");

		// SPTR - Script Pointers
		for (int32_t index(0); index < scriptCount; ++index)
			write_script(&chunkout, ObjectExpression::get_script(index));

		write_chunk(out, &chunkout, "SPTR");

		// SFLG - Script Flags
		for (int32_t index(0); index < scriptCount; ++index)
			write_script_flags(&chunkout, ObjectExpression::get_script(index));

		write_chunk(out, &chunkout, "SFLG");

		// STRL - String Literals
		if (stringCount)
		{
			write_32(&chunkout, 0);
			write_32(&chunkout, stringCount);
			write_32(&chunkout, 0);
		}

		for (int32_t index(0); index < stringCount; ++index)
			write_32(&chunkout, 12 + (stringCount*4) + ObjectExpression::get_string(index).offset);

		for (int32_t index(0); index < stringCount; ++index)
			write_string(&chunkout, ObjectExpression::get_string(index).string);

		write_chunk(out, &chunkout, "STRL");

		// SVCT - Script Variable Counts
		for (int32_t index(0); index < scriptCount; ++index)
			write_script_vars(&chunkout, ObjectExpression::get_script(index));

		write_chunk(out, &chunkout, "SVCT");

		break;

	default:
		throw SourceException("unknown output type", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_chunk(std::ostream * out, std::ostringstream * chunkout, char const * chunkname)
{
	std::string chunk(chunkout->str());

	if (chunk.size())
	{
		*out << chunkname;
		write_32(out, chunk.size());
		*out << chunk;
	}

	chunkout->str("");
}

void BinaryTokenZDACS::write_function(std::ostream * const out, ObjectExpression::Function const & f)
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
		throw SourceException("unknown output type for function", SourcePosition::none, "BinaryTokenZDACS");
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
		write_32(out, (int32_t)(s.stype * 1000) + (int32_t)s.number);
		write_32(out, *ObjectExpression::get_symbol(s.label, SourcePosition::none));
		write_32(out, s.argCount);
		break;

	case OUTPUT_ACSE:
		write_16(out, (int16_t)s.number);
		write_16(out, (int16_t)s.stype);
		write_32(out, *ObjectExpression::get_symbol(s.label, SourcePosition::none));
		write_32(out, s.argCount);
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
		write_16(out, (int16_t)s.varCount);
		break;

	default:
		throw SourceException("unknown output type for script_vars", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_string(std::ostream * const out, std::string const & s)
{
	*out << s;
}



