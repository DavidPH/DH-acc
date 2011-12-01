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

/* BinaryTokenPPACS.cpp
**
** Defines the BinaryTokenPPACS methods.
*/

#include "BinaryTokenPPACS.hpp"

#include "ACSP.hpp"
#include "BinaryTokenZDACS.hpp"
#include "ObjectExpression.hpp"
#include "ost_type.hpp"
#include "SourceException.hpp"

#include <sstream>



int BinaryTokenPPACS::_arg_counts[BCODE_NONE];



BinaryTokenPPACS::BinaryTokenPPACS(BinaryCode code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<ObjectExpression::Pointer> const & args) : _args(args), _code(code), _labels(labels), _position(position)
{

}

void BinaryTokenPPACS::addLabel(std::string const & label)
{
	_labels.push_back(label);
}

void BinaryTokenPPACS::init()
{
	#define DO_INIT(NAME,ARGC)\
	_arg_counts[BCODE_##NAME] = ARGC

	DO_INIT(NOP, 0);

	// Arithmetic
	DO_INIT(ADD,       0);
	DO_INIT(DIV,       0);
	DO_INIT(DIV_FIXED, 0);
	DO_INIT(MOD,       0);
	DO_INIT(MUL,       0);
	DO_INIT(MUL_FIXED, 0);
	DO_INIT(SUB,       0);

	// Bitwise
	DO_INIT(BITWISE_AND,    0);
	DO_INIT(BITWISE_IOR,    0);
	DO_INIT(BITWISE_SHIFTL, 0);
	DO_INIT(BITWISE_SHIFTR, 0);
	DO_INIT(BITWISE_XOR,    0);

	// Branching
	DO_INIT(BRANCH_CASE,     2);
	DO_INIT(BRANCH_GOTO,     0);
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
	DO_INIT(ADDR_STACK_ADD, 0);
	DO_INIT(ADDR_STACK_SUB, 0);
	DO_INIT(ADDR_STACK_VAR, 1);

	// Variable Get
	DO_INIT(GET_AUTO_VAR,          1);
	DO_INIT(GET_LITERAL,           1);
	DO_INIT(GET_MAPREGISTER_VAR,   1);
	DO_INIT(GET_MOBJ_VAR,          0);
	DO_INIT(GET_POINTER_VAR,       1);
	DO_INIT(GET_REGISTER_VAR,      1);
	DO_INIT(GET_STATIC_VAR,        1);
	DO_INIT(GET_WORLDREGISTER_VAR, 1);

	// Variable Set
	DO_INIT(SET_AUTO_VAR,          1);
	DO_INIT(SET_MAPREGISTER_VAR,   1);
	DO_INIT(SET_MOBJ_VAR,          0);
	DO_INIT(SET_POINTER_VAR,       1);
	DO_INIT(SET_REGISTER_VAR,      1);
	DO_INIT(SET_STATIC_VAR,        1);
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
	DO_INIT(GET_ACTIVATOR_MID, 0);
	DO_INIT(SET_ACTIVATOR_MID, 0);

	// ACS Printing
	DO_INIT(PRINT_CHARACTER, 0);
	DO_INIT(PRINT_END,       0);
	DO_INIT(PRINT_END_BOLD,  0);
	DO_INIT(PRINT_END_ERROR, 0);
	DO_INIT(PRINT_END_LOG,   0);
	DO_INIT(PRINT_FIXED,     0);
	DO_INIT(PRINT_NUM_DEC,   0);
	DO_INIT(PRINT_START,     0);
	DO_INIT(PRINT_STRING,    0);

	// Unsorted
	DO_INIT(ADD_WORLDREGISTER_VAR, 1);
	DO_INIT(SUB_WORLDREGISTER_VAR, 1);

	#undef DO_INIT
}

size_t BinaryTokenPPACS::size() const
{
	return _arg_counts[_code]*4 + 4;
}

void BinaryTokenPPACS::write(std::ostream * out) const
{
	std::ostringstream token;

	switch (output_type)
	{
	case OUTPUT_ACS0:
		BinaryTokenZDACS::write_32(out, _code);

		for (int i(0); i < _arg_counts[_code]; ++i)
		{
			if ((size_t)i < _args.size())
				BinaryTokenZDACS::write_32(out, *_args[i]);
			else
				BinaryTokenZDACS::write_32(out, 0);
		}
		break;

	case OUTPUT_ACSP:
		for (size_t i(0); i < _labels.size(); ++i)
			write_label(out, _labels[i]);

		BinaryTokenZDACS::write_32(&token, _code);

		for (int i(0); i < _arg_counts[_code]; ++i)
		{
			if ((size_t)i < _args.size())
				_args[i]->writeACSP(&token);
			else
			{
				BinaryTokenZDACS::write_32(&token, ACSP_EXPR_LITERAL);
				BinaryTokenZDACS::write_32(&token, 0);
			}
		}

		BinaryTokenZDACS::write_32(out, ACSP_TOKEN_INSTRUCTION);
		BinaryTokenZDACS::write_32(out, token.str().size());
		*out << token.str();
		break;

	default:
		throw SourceException("unknown output type for instruction", SourcePosition::none, "BinaryTokenPPACS");
	}
}

void BinaryTokenPPACS::write_all(std::ostream * out, std::vector<BinaryTokenPPACS> const & instructions)
{
	ObjectExpression::set_address_count(8);

	for (std::vector<BinaryTokenPPACS>::const_iterator instr(instructions.begin()); instr != instructions.end(); ++instr)
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
		BinaryTokenZDACS::write_32(out, ObjectExpression::get_address_count());

		// 8
		for (std::vector<BinaryTokenPPACS>::const_iterator instr(instructions.begin()); instr != instructions.end(); ++instr)
			instr->write(out);

		// directoryOffset
		BinaryTokenZDACS::write_32(out, scriptCount);

		// directoryOffset+4
		ObjectExpression::iter_script(write_script, out);

		// directoryOffset+4+(scriptCount*12)
		BinaryTokenZDACS::write_32(out, stringCount);

		// directoryOffset+4+(scriptCount*12)+4
		ObjectExpression::iter_string(write_string_offset, out);

		// directoryOffset+4+(scriptCount*12)+4+(stringCount*4)
		ObjectExpression::iter_string(write_string, out);

		break;

	case OUTPUT_ACSP:
		// 0
		*out << 'A' << 'C' << 'S' << '+';

		for (std::vector<BinaryTokenPPACS>::const_iterator instr(instructions.begin()); instr != instructions.end(); ++instr)
			instr->write(out);

		ObjectExpression::iter_auto(write_auto, out);
		ObjectExpression::iter_script(write_script, out);
		ObjectExpression::iter_static(write_static, out);
		ObjectExpression::iter_string(write_string, out);

		BinaryTokenZDACS::write_32(out, ACSP_TOKEN_EOF);
		BinaryTokenZDACS::write_32(out, 0);

		break;

	default:
		throw SourceException("unknown output type", SourcePosition::none, "BinaryTokenPPACS");
	}
}

void BinaryTokenPPACS::write_auto(std::ostream * out, ObjectData_Auto const & a)
{
	std::ostringstream token;

	switch (output_type)
	{
	case OUTPUT_ACSP:
		write_string(&token, a.name);
		BinaryTokenZDACS::write_32(&token, ACSP_EXPR_LITERAL);
		BinaryTokenZDACS::write_32(&token, a.number);

		BinaryTokenZDACS::write_32(out, ACSP_TOKEN_SYMBOL);
		BinaryTokenZDACS::write_32(out, token.str().size());
		*out << token.str();
		break;

	default:
		throw SourceException("unknown output type for string_offset", SourcePosition::none, "BinaryTokenPPACS");
	}
}

void BinaryTokenPPACS::write_label(std::ostream * out, std::string const & label)
{
	std::ostringstream token;

	switch (output_type)
	{
	case OUTPUT_ACSP:
		write_string(&token, label);

		BinaryTokenZDACS::write_32(out, ACSP_TOKEN_LABEL);
		BinaryTokenZDACS::write_32(out, token.str().size());
		*out << token.str();
		break;

	default:
		throw SourceException("unknown output type for label", SourcePosition::none, "BinaryTokenPPACS");
	}
}

void BinaryTokenPPACS::write_script(std::ostream * out, ObjectData_Script const & s)
{
	std::ostringstream token;

	switch (output_type)
	{
	case OUTPUT_ACS0:
		BinaryTokenZDACS::write_32(out, (s.stype * 1000) + s.number);
		BinaryTokenZDACS::write_32(out, *ObjectExpression::get_symbol(s.label, SourcePosition::none));
		BinaryTokenZDACS::write_32(out, s.argCount <= 3 ? s.argCount : 3);
		break;

	case OUTPUT_ACSP:
		write_string(&token, s.name);
		BinaryTokenZDACS::write_32(&token, ACSP_EXPR_SYMBOL);
		write_string(&token, s.label);
		BinaryTokenZDACS::write_32(&token, s.number);
		BinaryTokenZDACS::write_32(&token, s.stype);
		BinaryTokenZDACS::write_32(&token, s.flags);
		BinaryTokenZDACS::write_32(&token, s.argCount);
		BinaryTokenZDACS::write_32(&token, s.varCount);

		BinaryTokenZDACS::write_32(out, ACSP_TOKEN_SCRIPT);
		BinaryTokenZDACS::write_32(out, token.str().size());
		*out << token.str();
		break;

	default:
		throw SourceException("unknown output type for script", SourcePosition::none, "BinaryTokenPPACS");
	}
}

void BinaryTokenPPACS::write_static(std::ostream * out, ObjectData_Static const & s)
{
	std::ostringstream token;

	switch (output_type)
	{
	case OUTPUT_ACSP:
		write_string(&token, s.name);

		if (s.number == -1)
		{
			BinaryTokenZDACS::write_32(&token, s.size);
			BinaryTokenZDACS::write_32(out, ACSP_TOKEN_ALLOCATE);
		}
		else
		{
			BinaryTokenZDACS::write_32(&token, ACSP_EXPR_LITERAL);
			BinaryTokenZDACS::write_32(&token, s.number);
			BinaryTokenZDACS::write_32(out, ACSP_TOKEN_SYMBOL);
		}

		BinaryTokenZDACS::write_32(out, token.str().size());
		*out << token.str();
		break;

	default:
		throw SourceException("unknown output type for string_offset", SourcePosition::none, "BinaryTokenPPACS");
	}
}

void BinaryTokenPPACS::write_string(std::ostream * out, ObjectData_String const & s)
{
	std::ostringstream token;

	switch (output_type)
	{
	case OUTPUT_ACS0:
		write_string(out, s.string);
		break;

	case OUTPUT_ACSP:
		write_string(&token, s.name);
		write_string(&token, s.string);

		BinaryTokenZDACS::write_32(out, ACSP_TOKEN_STRING);
		BinaryTokenZDACS::write_32(out, token.str().size());
		*out << token.str();
		break;

	default:
		throw SourceException("unknown output type for string_offset", SourcePosition::none, "BinaryTokenPPACS");
	}
}
void BinaryTokenPPACS::write_string(std::ostream * out, std::string const & s)
{
	size_t extra;
	size_t length;

	switch (output_type)
	{
	case OUTPUT_ACS0:
		*out << s;
		break;

	case OUTPUT_ACSP:
		length = (s.size() + 4) & ~3;
		extra = length - s.size();
		BinaryTokenZDACS::write_32(out, length);
		*out << s;
		while (extra--) *out << '\0';
		break;

	default:
		throw SourceException("unknown output type for string_offset", SourcePosition::none, "BinaryTokenPPACS");
	}
}
void BinaryTokenPPACS::write_string_offset(std::ostream * out, ObjectData_String const & s)
{
	switch (output_type)
	{
	case OUTPUT_ACS0:
		BinaryTokenZDACS::write_32(out, ObjectExpression::get_address_count() + 4 + (ObjectExpression::get_script_count() * 12) + 4 + (ObjectExpression::get_string_count() * 4) + s.offset);
		break;

	default:
		throw SourceException("unknown output type for string_offset", SourcePosition::none, "BinaryTokenPPACS");
	}
}


