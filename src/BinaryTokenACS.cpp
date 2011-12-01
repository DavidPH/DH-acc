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

/* BinaryTokenACS.cpp
**
** Defines the BinaryTokenACS methods.
*/

#include "BinaryTokenACS.hpp"

#include "ObjectExpression.hpp"
#include "ost_type.hpp"
#include "SourceException.hpp"

#include <sstream>



int BinaryTokenACS::_arg_counts[BCODE_NONE];



BinaryTokenACS::BinaryTokenACS(BinaryCode code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<ObjectExpression::Pointer> const & args) : _args(args), _code(code), _labels(labels), _position(position)
{

}

void BinaryTokenACS::addLabel(std::string const & label)
{
	_labels.push_back(label);
}

void BinaryTokenACS::init()
{
	#define DO_INIT(NAME,ARGC)\
	_arg_counts[BCODE_##NAME] = ARGC

	DO_INIT(NOP, 0);

	// Arithmetic
	DO_INIT(ADD, 0);
	DO_INIT(DIV, 0);
	DO_INIT(MOD, 0);
	DO_INIT(MUL, 0);
	DO_INIT(SUB, 0);

	// Bitwise
	DO_INIT(BITWISE_AND,    0);
	DO_INIT(BITWISE_IOR,    0);
	DO_INIT(BITWISE_SHIFTL, 0);
	DO_INIT(BITWISE_SHIFTR, 0);
	DO_INIT(BITWISE_XOR,    0);

	// Branching
	DO_INIT(BRANCH_CASE,     2);
	DO_INIT(BRANCH_GOTO_IMM, 1);
	DO_INIT(BRANCH_TRUE,     1);
	DO_INIT(BRANCH_ZERO,     1);

	// Comparison
	DO_INIT(CMP_EQ, 0);
	DO_INIT(CMP_GE, 0);
	DO_INIT(CMP_GT, 0);
	DO_INIT(CMP_LE, 0);
	DO_INIT(CMP_LT, 0);
	DO_INIT(CMP_NE, 0);

	// Logical
	DO_INIT(LOGICAL_AND, 0);
	DO_INIT(LOGICAL_IOR, 0);
	DO_INIT(LOGICAL_NOT, 0);

	// Stack-ops
	DO_INIT(STACK_DROP, 0);

	// Trigonometry

	// Variable Address

	// Variable Get
	DO_INIT(GET_LITERAL,           1);
	DO_INIT(GET_MAPREGISTER_VAR,   1);
	DO_INIT(GET_REGISTER_VAR,      1);
	DO_INIT(GET_WORLDREGISTER_VAR, 1);

	// Variable Set
	DO_INIT(SET_MAPREGISTER_VAR,   1);
	DO_INIT(SET_REGISTER_VAR,      1);
	DO_INIT(SET_WORLDREGISTER_VAR, 1);

	// Miscellaneous
	DO_INIT(MISC_DELAY,      0);
	DO_INIT(MISC_DELAY_IMM,  2);
	DO_INIT(MISC_NEGATE,     0);
	DO_INIT(MISC_RANDOM,     0);
	DO_INIT(MISC_RANDOM_IMM, 2);

	// ACS
	DO_INIT(LINESPEC1,        1);
	DO_INIT(LINESPEC1_IMM,    2);
	DO_INIT(LINESPEC2,        1);
	DO_INIT(LINESPEC2_IMM,    3);
	DO_INIT(LINESPEC3,        1);
	DO_INIT(LINESPEC3_IMM,    4);
	DO_INIT(LINESPEC4,        1);
	DO_INIT(LINESPEC4_IMM,    5);
	DO_INIT(LINESPEC5,        1);
	DO_INIT(LINESPEC5_IMM,    6);
	DO_INIT(SCRIPT_RESTART,   0);
	DO_INIT(SCRIPT_SUSPEND,   0);
	DO_INIT(SCRIPT_TERMINATE, 0);
	DO_INIT(THINGCOUNT,       0);
	DO_INIT(THINGCOUNT_IMM,   2);
	DO_INIT(TIMER,            0);

	// ACS Common Extensions

	// ACS Extensions

	// ACS Printing
	DO_INIT(PRINT_CHARACTER, 0);
	DO_INIT(PRINT_END,       0);
	DO_INIT(PRINT_END_BOLD,  0);
	DO_INIT(PRINT_NUM_DEC,   0);
	DO_INIT(PRINT_START,     0);
	DO_INIT(PRINT_STRING,    0);

	// Other

	// Unsorted
	DO_INIT(ADD_WORLDREGISTER_VAR, 1);
	DO_INIT(SUB_WORLDREGISTER_VAR, 1);

	#undef DO_INIT
}

size_t BinaryTokenACS::size() const
{
	return _arg_counts[_code]*4 + 4;
}

void BinaryTokenACS::write(std::ostream * const out) const
{
	write_32(out, _code);

	for (int i(0); i < _arg_counts[_code]; ++i)
	{
		if ((size_t)i < _args.size())
			write_32(out, *_args[i]);
		else
			write_32(out, 0);
	}
}

void BinaryTokenACS::write_8(std::ostream * out, bigsint i)
{
	out->put(char((i >> 0) & 0xFF));
}
void BinaryTokenACS::write_16(std::ostream * out, bigsint i)
{
	out->put(char((i >> 0) & 0xFF));
	out->put(char((i >> 8) & 0xFF));
}
void BinaryTokenACS::write_32(std::ostream * const out, ObjectExpression const & expr)
{
	switch (expr.getType())
	{
	case ObjectExpression::ET_FLOAT:
		write_32(out, (bigsint)(expr.resolveFloat() * 65536.0));
		break;

	case ObjectExpression::ET_INT:
		write_32(out, expr.resolveInt());
		break;

	default:
		throw SourceException("invalid ET", SourcePosition::none, "BinaryTokenZDACS::write_32");
	}
}
void BinaryTokenACS::write_32(std::ostream * out, bigsint i)
{
	out->put(char((i >>  0) & 0xFF));
	out->put(char((i >>  8) & 0xFF));
	out->put(char((i >> 16) & 0xFF));
	out->put(char((i >> 24) & 0xFF));
}

void BinaryTokenACS::write_all(std::ostream * const out, std::vector<BinaryTokenACS> const & instructions)
{
	ObjectExpression::set_address_count(8);

	for (std::vector<BinaryTokenACS>::const_iterator instr(instructions.begin()); instr != instructions.end(); ++instr)
	{
		for (std::vector<std::string>::const_iterator label(instr->_labels.begin()); label != instr->_labels.end(); ++label)
			ObjectExpression::add_label(*label);

		ObjectExpression::add_address_count(instr->size());
	}

	bigsint const scriptCount(ObjectExpression::get_script_count());
	bigsint const stringCount(ObjectExpression::get_string_count());

	switch (output_type)
	{
	case OUTPUT_ACS0:
		// 0
		*out << 'A' << 'C' << 'S' << '\0';
		write_32(out, ObjectExpression::get_address_count());

		// 8
		for (std::vector<BinaryTokenACS>::const_iterator instr(instructions.begin()); instr != instructions.end(); ++instr)
			instr->write(out);

		// directoryOffset
		write_32(out, scriptCount);

		// directoryOffset+4
		ObjectExpression::iter_script(write_script, out);

		// directoryOffset+4+(scriptCount*12)
		write_32(out, stringCount);

		// directoryOffset+4+(scriptCount*12)+4
		ObjectExpression::iter_string(write_string_offset, out);

		// directoryOffset+4+(scriptCount*12)+4+(stringCount*4)
		ObjectExpression::iter_string(write_string, out);

		break;

	default:
		throw SourceException("unknown output type", SourcePosition::none, "BinaryTokenACS");
	}
}

void BinaryTokenACS::write_script(std::ostream * out, ObjectData_Script const & s)
{
	switch (output_type)
	{
	case OUTPUT_ACS0:
		write_32(out, (s.stype * 1000) + s.number);
		write_32(out, *ObjectExpression::get_symbol(s.label, SourcePosition::none));
		write_32(out, s.argCount <= 3 ? s.argCount : 3);
		break;

	default:
		throw SourceException("unknown output type for script", SourcePosition::none, "BinaryTokenACS");
	}
}

void BinaryTokenACS::write_string(std::ostream * out, ObjectData_String const & s)
{
	write_string(out, s.string);
}
void BinaryTokenACS::write_string(std::ostream * const out, std::string const & s)
{
	*out << s;
}
void BinaryTokenACS::write_string_offset(std::ostream * out, ObjectData_String const & s)
{
	switch (output_type)
	{
	case OUTPUT_ACS0:
		write_32(out, ObjectExpression::get_address_count() + 4 + (ObjectExpression::get_script_count() * 12) + 4 + (ObjectExpression::get_string_count() * 4) + s.offset);
		break;

	default:
		throw SourceException("unknown output type for string_offset", SourcePosition::none, "BinaryTokenACS");
	}
}


