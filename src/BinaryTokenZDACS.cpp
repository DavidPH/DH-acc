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

#include "ObjectExpression.hpp"
#include "ost_type.hpp"
#include "SourceException.hpp"



int BinaryTokenZDACS::_arg_counts[BCODE_NONE];
bigsint BinaryTokenZDACS::_temp_counter;



BinaryTokenZDACS::BinaryTokenZDACS(BinaryCode code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<ObjectExpression::Pointer> const & args) : _args(args), _code(code), _labels(labels), _position(position)
{

}

void BinaryTokenZDACS::addLabel(std::string const & label)
{
	_labels.push_back(label);
}

void BinaryTokenZDACS::init()
{
	BinaryTokenACS::init(_arg_counts);

	#define DO_INIT(NAME,ARGC)\
	_arg_counts[BCODE_##NAME] = ARGC

	// Arithmetic
	DO_INIT(DIV_FIXED, 0);
	DO_INIT(MUL_FIXED, 0);

	// Bitwise
	DO_INIT(BITWISE_NOT, 0);

	// Branching

	// Comparison

	// Logical

	// Stack-ops
	DO_INIT(STACK_DUP,  0);
	DO_INIT(STACK_SWAP, 0);

	// Trigonometry
	DO_INIT(TRIG_COS, 0);
	DO_INIT(TRIG_SIN, 0);

	// Variable Address

	// Variable Get
	DO_INIT(GET_GLOBALARRAY_VAR,    1);
	DO_INIT(GET_GLOBALREGISTER_VAR, 1);
	DO_INIT(GET_MAPARRAY_VAR,       1);
	DO_INIT(GET_TID_VAR,            0);
	DO_INIT(GET_WORLDARRAY_VAR,     1);

	// Variable Set
	DO_INIT(SET_GLOBALARRAY_VAR,    1);
	DO_INIT(SET_GLOBALREGISTER_VAR, 1);
	DO_INIT(SET_MAPARRAY_VAR,       1);
	DO_INIT(SET_TID_VAR,            0);
	DO_INIT(SET_WORLDARRAY_VAR,     1);

	// Miscellaneous
	DO_INIT(MISC_NATIVE, 2);
	DO_INIT(MISC_STRLEN, 0);

	// ACS

	// ACS Common Extensions

	// ACS Extensions
	DO_INIT(CALLFUNC_IMM,        1);
	DO_INIT(CALLFUNCVOID_IMM,    1);
	DO_INIT(GET_ACTIVATOR_TID,   0);
	DO_INIT(GET_AMMO_CAPACITY,   0);
	DO_INIT(GET_TID_ANGLE,       0);
	DO_INIT(GET_TID_PITCH,       0);
	DO_INIT(GET_TID_X,           0);
	DO_INIT(GET_TID_Y,           0);
	DO_INIT(GET_TID_Z,           0);
	DO_INIT(INVENTORY_CHECK,     0);
	DO_INIT(INVENTORY_CHECK_IMM, 1);
	DO_INIT(INVENTORY_GIVE,      0);
	DO_INIT(INVENTORY_GIVE_IMM,  2);
	DO_INIT(INVENTORY_TAKE,      0);
	DO_INIT(INVENTORY_TAKE_IMM,  2);
	DO_INIT(LINESPEC5RESULT,     1);
	DO_INIT(PLAYERNUMBER,        0);
	DO_INIT(RETNFUNC,            0);
	DO_INIT(RETNFUNCVOID,        0);
	DO_INIT(SCRIPT_SETRETURN,    0);
	DO_INIT(SET_AMMO_CAPACITY,   0);
	DO_INIT(SET_TID_ANGLE,       0);
	DO_INIT(SET_TID_PITCH,       0);
	DO_INIT(SET_TID_POSITION,    0);
	DO_INIT(SPAWN,               0);
	DO_INIT(SPAWN_IMM,           6);
	DO_INIT(SPAWN_PROJECTILE,    0);

	// ACS Printing
	DO_INIT(PRINT_END_LOG,   0);
	DO_INIT(PRINT_FIXED,     0);
	DO_INIT(PRINT_NUM_HEX,   0);

	// Other
	DO_INIT(BRANCH_TABLE, -1);

	// Unsorted

	#undef DO_INIT
}

BinaryTokenZDACS::label_iterator BinaryTokenZDACS::label_begin() const
{
	return _labels.begin();
}
BinaryTokenZDACS::label_iterator BinaryTokenZDACS::label_end() const
{
	return _labels.end();
}

size_t BinaryTokenZDACS::size() const
{
	if (_arg_counts[_code] < 0) switch (_code)
	{
	case BCODE_BRANCH_TABLE:
		return _args.size()*6 + 4;

	default:
		throw SourceException("???", SourcePosition::none, "BinaryTokenZDACS::size");
	}
	else
		return _arg_counts[_code]*4 + 4;
}

void BinaryTokenZDACS::writeACS0(std::ostream * out) const
{
	if (_arg_counts[_code] < 0) switch (_code)
	{
	case BCODE_BRANCH_TABLE:
		// TODO: BCODE_BRANCHCASESORTED

		for (size_t i(0); i < _args.size(); i += 2)
		{
			BinaryTokenACS::write_ACS0_32(out, BCODE_BRANCH_CASE);
			BinaryTokenACS::write_ACS0_32(out, *_args[i+0]);
			BinaryTokenACS::write_ACS0_32(out, *_args[i+1]);
		}

		BinaryTokenACS::write_ACS0_32(out, BCODE_STACK_DROP);

		break;

	default:
		throw SourceException("???", SourcePosition::none, "BinaryTokenZDACS::write");
	}
	else
	{
		BinaryTokenACS::write_ACS0_32(out, _code);

		for (int i(0); i < _arg_counts[_code]; ++i)
		{
			if ((size_t)i < _args.size())
				BinaryTokenACS::write_ACS0_32(out, *_args[i]);
			else
				BinaryTokenACS::write_ACS0_32(out, 0);
		}
	}
}

void BinaryTokenZDACS::write_all(std::ostream * out, std::vector<BinaryTokenZDACS> const & instructions)
{
	BinaryTokenACS::output_prep(instructions);

	switch (output_type)
	{
	case OUTPUT_ACS0:
		BinaryTokenACS::output_ACS0(out, instructions);
		break;

	case OUTPUT_ACSE:
		output_ACSE(out, instructions);
		break;

	default:
		throw SourceException("unknown output type", SourcePosition::none, "BinaryTokenZDACS");
	}
}


