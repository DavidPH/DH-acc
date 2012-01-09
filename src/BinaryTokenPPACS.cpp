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
	BinaryTokenACS::init(_arg_counts);

	#define DO_INIT(NAME,ARGC)\
	_arg_counts[BCODE_##NAME] = ARGC

	// Arithmetic
	DO_INIT(DIV_FIXED, 0);
	DO_INIT(MUL_FIXED, 0);

	// Bitwise

	// Branching
	DO_INIT(BRANCH_GOTO, 0);

	// Comparison

	// Logical

	// Stack-ops

	// Trigonometry

	// Variable Address
	DO_INIT(ADDR_STACK_ADD, 0);
	DO_INIT(ADDR_STACK_SUB, 0);
	DO_INIT(ADDR_STACK_VAR, 1);

	// Variable Get
	DO_INIT(GET_AUTO_VAR,    1);
	DO_INIT(GET_MOBJ_VAR,    0);
	DO_INIT(GET_POINTER_VAR, 1);
	DO_INIT(GET_STATIC_VAR,  1);

	// Variable Set
	DO_INIT(SET_AUTO_VAR,    1);
	DO_INIT(SET_MOBJ_VAR,    0);
	DO_INIT(SET_POINTER_VAR, 1);
	DO_INIT(SET_STATIC_VAR,  1);

	// Variable Set Op
	DO_INIT(SETOP_ADD_AUTO_VAR,    1);
	DO_INIT(SETOP_ADD_POINTER_VAR, 1);
	DO_INIT(SETOP_ADD_STATIC_VAR,  1);
	DO_INIT(SETOP_SUB_AUTO_VAR,    1);
	DO_INIT(SETOP_SUB_POINTER_VAR, 1);
	DO_INIT(SETOP_SUB_STATIC_VAR,  1);
	DO_INIT(SETOP_MUL_AUTO_VAR,    1);
	DO_INIT(SETOP_MUL_POINTER_VAR, 1);
	DO_INIT(SETOP_MUL_STATIC_VAR,  1);
	DO_INIT(SETOP_DIV_AUTO_VAR,    1);
	DO_INIT(SETOP_DIV_POINTER_VAR, 1);
	DO_INIT(SETOP_DIV_STATIC_VAR,  1);
	DO_INIT(SETOP_MOD_AUTO_VAR,    1);
	DO_INIT(SETOP_MOD_POINTER_VAR, 1);
	DO_INIT(SETOP_MOD_STATIC_VAR,  1);
	DO_INIT(SETOP_INC_AUTO_VAR,    1);
	DO_INIT(SETOP_INC_POINTER_VAR, 1);
	DO_INIT(SETOP_INC_STATIC_VAR,  1);
	DO_INIT(SETOP_DEC_AUTO_VAR,    1);
	DO_INIT(SETOP_DEC_POINTER_VAR, 1);
	DO_INIT(SETOP_DEC_STATIC_VAR,  1);

	// Miscellaneous

	// ACS

	// ACS Common Extensions

	// ACS Extensions
	DO_INIT(GET_ACTIVATOR_MID, 0);
	DO_INIT(SET_ACTIVATOR_MID, 0);

	// ACS Printing
	DO_INIT(PRINT_END_ERROR, 0);
	DO_INIT(PRINT_END_LOG,   0);
	DO_INIT(PRINT_FIXED,     0);

	// Unsorted

	#undef DO_INIT
}

BinaryTokenPPACS::label_iterator BinaryTokenPPACS::label_begin() const
{
	return _labels.begin();
}
BinaryTokenPPACS::label_iterator BinaryTokenPPACS::label_end() const
{
	return _labels.end();
}

size_t BinaryTokenPPACS::size() const
{
	return _arg_counts[_code]*4 + 4;
}

void BinaryTokenPPACS::writeACS0(std::ostream * out) const
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
void BinaryTokenPPACS::writeACSP(std::ostream * out) const
{
	std::ostringstream token;

	for (size_t i(0); i < _labels.size(); ++i)
		write_ACSP_label(out, _labels[i]);

	BinaryTokenACS::write_ACS0_32(&token, _code);

	for (int i(0); i < _arg_counts[_code]; ++i)
	{
		if ((size_t)i < _args.size())
			_args[i]->writeACSP(&token);
		else
		{
			BinaryTokenACS::write_ACS0_32(&token, ACSP_EXPR_LITERAL);
			BinaryTokenACS::write_ACS0_32(&token, 0);
		}
	}

	BinaryTokenACS::write_ACS0_32(out, ACSP_TOKEN_INSTRUCTION);
	BinaryTokenACS::write_ACS0_32(out, token.str().size());
	*out << token.str();
}

void BinaryTokenPPACS::write_all(std::ostream * out, std::vector<BinaryTokenPPACS> const & instructions)
{
	BinaryTokenACS::output_prep(instructions);

	switch (output_type)
	{
	case OUTPUT_ACS0:
		BinaryTokenACS::output_ACS0(out, instructions);
		break;

	case OUTPUT_ACSP:
		output_ACSP(out, instructions);
		break;

	default:
		throw SourceException("unknown output type", SourcePosition::none(), "BinaryTokenPPACS");
	}
}


