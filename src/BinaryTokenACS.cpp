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
	init(_arg_counts);
}
void BinaryTokenACS::init(int * argCounts)
{
	#define DO_INIT(NAME,ARGC)\
	argCounts[BCODE_##NAME] = ARGC

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

	// Variable Set Op
	DO_INIT(SETOP_ADD_MAPREGISTER_VAR,   1);
	DO_INIT(SETOP_ADD_REGISTER_VAR,      1);
	DO_INIT(SETOP_ADD_WORLDREGISTER_VAR, 1);
	DO_INIT(SETOP_SUB_MAPREGISTER_VAR,   1);
	DO_INIT(SETOP_SUB_REGISTER_VAR,      1);
	DO_INIT(SETOP_SUB_WORLDREGISTER_VAR, 1);
	DO_INIT(SETOP_MUL_MAPREGISTER_VAR,   1);
	DO_INIT(SETOP_MUL_REGISTER_VAR,      1);
	DO_INIT(SETOP_MUL_WORLDREGISTER_VAR, 1);
	DO_INIT(SETOP_DIV_MAPREGISTER_VAR,   1);
	DO_INIT(SETOP_DIV_REGISTER_VAR,      1);
	DO_INIT(SETOP_DIV_WORLDREGISTER_VAR, 1);
	DO_INIT(SETOP_MOD_MAPREGISTER_VAR,   1);
	DO_INIT(SETOP_MOD_REGISTER_VAR,      1);
	DO_INIT(SETOP_MOD_WORLDREGISTER_VAR, 1);
	DO_INIT(SETOP_INC_MAPREGISTER_VAR,   1);
	DO_INIT(SETOP_INC_REGISTER_VAR,      1);
	DO_INIT(SETOP_INC_WORLDREGISTER_VAR, 1);
	DO_INIT(SETOP_DEC_MAPREGISTER_VAR,   1);
	DO_INIT(SETOP_DEC_REGISTER_VAR,      1);
	DO_INIT(SETOP_DEC_WORLDREGISTER_VAR, 1);

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

	#undef DO_INIT
}

BinaryTokenACS::label_iterator BinaryTokenACS::label_begin() const
{
	return _labels.begin();
}
BinaryTokenACS::label_iterator BinaryTokenACS::label_end() const
{
	return _labels.end();
}

size_t BinaryTokenACS::size() const
{
	return _arg_counts[_code]*4 + 4;
}

void BinaryTokenACS::writeACS0(std::ostream * const out) const
{
	write_ACS0_32(out, _code);

	for (int i(0); i < _arg_counts[_code]; ++i)
	{
		if ((size_t)i < _args.size())
			write_ACS0_32(out, *_args[i]);
		else
			write_ACS0_32(out, 0);
	}
}

void BinaryTokenACS::write_all(std::ostream * out, std::vector<BinaryTokenACS> const & instructions)
{
	output_prep(instructions);

	switch (output_type)
	{
	case OUTPUT_ACS0:
		output_ACS0(out, instructions);
		break;

	default:
		throw SourceException("unknown output type", SourcePosition::none(), "BinaryTokenACS");
	}
}


