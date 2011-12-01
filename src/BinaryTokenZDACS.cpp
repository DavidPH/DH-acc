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

#include <sstream>



int BinaryTokenZDACS::_arg_counts[BCODE_NONE];

static bigsint string_offset;



BinaryTokenZDACS::BinaryTokenZDACS(BinaryCode code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<ObjectExpression::Pointer> const & args) : _args(args), _code(code), _labels(labels), _position(position)
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
	DO_INIT(BITWISE_NOT,    0);
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
	DO_INIT(STACK_DUP,  0);
	DO_INIT(STACK_SWAP, 0);

	// Trigonometry
	DO_INIT(TRIG_COS, 0);
	DO_INIT(TRIG_SIN, 0);

	// Variable Address

	// Variable Get
	DO_INIT(GET_GLOBALARRAY_VAR,    1);
	DO_INIT(GET_GLOBALREGISTER_VAR, 1);
	DO_INIT(GET_LITERAL,            1);
	DO_INIT(GET_MAPARRAY_VAR,       1);
	DO_INIT(GET_MAPREGISTER_VAR,    1);
	DO_INIT(GET_REGISTER_VAR,       1);
	DO_INIT(GET_TID_VAR,            0);
	DO_INIT(GET_WORLDARRAY_VAR,     1);
	DO_INIT(GET_WORLDREGISTER_VAR,  1);

	// Variable Set
	DO_INIT(SET_GLOBALARRAY_VAR,    1);
	DO_INIT(SET_GLOBALREGISTER_VAR, 1);
	DO_INIT(SET_MAPARRAY_VAR,       1);
	DO_INIT(SET_MAPREGISTER_VAR,    1);
	DO_INIT(SET_REGISTER_VAR,       1);
	DO_INIT(SET_TID_VAR,            0);
	DO_INIT(SET_WORLDARRAY_VAR,     1);
	DO_INIT(SET_WORLDREGISTER_VAR,  1);

	// Miscellaneous
	DO_INIT(MISC_DELAY,      0);
	DO_INIT(MISC_DELAY_IMM,  2);
	DO_INIT(MISC_NATIVE,     2);
	DO_INIT(MISC_NEGATE,     0);
	DO_INIT(MISC_RANDOM,     0);
	DO_INIT(MISC_RANDOM_IMM, 2);
	DO_INIT(MISC_STRLEN,     0);

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
	DO_INIT(PRINT_CHARACTER, 0);
	DO_INIT(PRINT_END,       0);
	DO_INIT(PRINT_END_BOLD,  0);
	DO_INIT(PRINT_END_LOG,   0);
	DO_INIT(PRINT_FIXED,     0);
	DO_INIT(PRINT_NUM_DEC,   0);
	DO_INIT(PRINT_NUM_HEX,   0);
	DO_INIT(PRINT_START,     0);
	DO_INIT(PRINT_STRING,    0);

	// Other
	DO_INIT(BRANCH_TABLE, -1);

	// Unsorted
	DO_INIT(ADD_WORLDREGISTER_VAR, 1);
	DO_INIT(SUB_WORLDREGISTER_VAR, 1);

	#undef DO_INIT
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

void BinaryTokenZDACS::write(std::ostream * const out) const
{
	if (_arg_counts[_code] < 0) switch (_code)
	{
	case BCODE_BRANCH_TABLE:
		// TODO: BCODE_BRANCHCASESORTED

		for (size_t i(0); i < _args.size(); i += 2)
		{
			write_32(out, BCODE_BRANCH_CASE);
			write_32(out, *_args[i+0]);
			write_32(out, *_args[i+1]);
		}

		write_32(out, BCODE_STACK_DROP);

		break;

	default:
		throw SourceException("???", SourcePosition::none, "BinaryTokenZDACS::write");
	}
	else
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
}

void BinaryTokenZDACS::write_8(std::ostream * out, bigsint i)
{
	out->put(char((i >> 0) & 0xFF));
}
void BinaryTokenZDACS::write_16(std::ostream * out, bigsint i)
{
	out->put(char((i >> 0) & 0xFF));
	out->put(char((i >> 8) & 0xFF));
}
void BinaryTokenZDACS::write_32(std::ostream * const out, ObjectExpression const & expr)
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
void BinaryTokenZDACS::write_32(std::ostream * out, bigsint i)
{
	out->put(char((i >>  0) & 0xFF));
	out->put(char((i >>  8) & 0xFF));
	out->put(char((i >> 16) & 0xFF));
	out->put(char((i >> 24) & 0xFF));
}

void BinaryTokenZDACS::write_all(std::ostream * const out, std::vector<BinaryTokenZDACS> const & instructions)
{
	ObjectExpression::set_address_count(8);

	for (std::vector<BinaryTokenZDACS>::const_iterator instr(instructions.begin()); instr != instructions.end(); ++instr)
	{
		for (std::vector<std::string>::const_iterator label(instr->_labels.begin()); label != instr->_labels.end(); ++label)
			ObjectExpression::add_label(*label);

		ObjectExpression::add_address_count(instr->size());
	}

	std::ostringstream chunkout;

	bigsint const scriptCount(ObjectExpression::get_script_count());
	bigsint const stringCount(ObjectExpression::get_string_count());

	switch (output_type)
	{
	case OUTPUT_ACS0:
		// 0
		*out << 'A' << 'C' << 'S' << '\0';
		write_32(out, ObjectExpression::get_address_count());

		// 8
		for (std::vector<BinaryTokenZDACS>::const_iterator instr(instructions.begin()); instr != instructions.end(); ++instr)
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

	case OUTPUT_ACSE:
		// Header
		*out << 'A' << 'C' << 'S' << 'E';
		write_32(out, ObjectExpression::get_address_count());

		// Instructions
		for (std::vector<BinaryTokenZDACS>::const_iterator instr(instructions.begin()); instr != instructions.end(); ++instr)
			instr->write(out);

		// ARAY - Map Array Declarations
		ObjectExpression::iter_registerarray_map(write_registerarray, &chunkout);
		write_chunk(out, &chunkout, "ARAY");

		// FNAM - Function Names
		string_offset = 0;
		ObjectExpression::iter_function(write_function_name_count);

		if (string_offset)
			write_32(&chunkout, string_offset/4);

		string_offset += 4;

		ObjectExpression::iter_function(write_function_name_offset, &chunkout);
		ObjectExpression::iter_function(write_function_name, &chunkout);
		write_chunk(out, &chunkout, "FNAM");

		// FUNC - Functions
		ObjectExpression::iter_function(write_function, &chunkout);
		write_chunk(out, &chunkout, "FUNC");

		// SPTR - Script Pointers
		ObjectExpression::iter_script(write_script, &chunkout);
		write_chunk(out, &chunkout, "SPTR");

		// SFLG - Script Flags
		ObjectExpression::iter_script(write_script_flags, &chunkout);
		write_chunk(out, &chunkout, "SFLG");

		// STRL - String Literals
		if (stringCount)
		{
			write_32(&chunkout, 0);
			write_32(&chunkout, stringCount);
			write_32(&chunkout, 0);
		}

		ObjectExpression::iter_string(write_string_offset, &chunkout);
		ObjectExpression::iter_string(write_string, &chunkout);
		write_chunk(out, &chunkout, "STRL");

		// SVCT - Script Variable Counts
		ObjectExpression::iter_script(write_script_vars, &chunkout);
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

void BinaryTokenZDACS::write_function(std::ostream * out, ObjectData_Function const & f)
{
	switch (output_type)
	{
	case OUTPUT_ACSE:
		write_8 (out, f.argCount);
		write_8 (out, f.varCount);
		write_8 (out, !!f.retCount);
		write_8 (out, 0);
		write_32(out, *ObjectExpression::get_symbol(f.label, SourcePosition::none));
		break;

	default:
		throw SourceException("unknown output type for function", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_function_name(std::ostream * out, ObjectData_Function const & f)
{
	switch (output_type)
	{
	case OUTPUT_ACSE:
		if (!f.external)
			*out << f.name << '\0';
		break;

	default:
		throw SourceException("unknown output type for function_name", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_function_name_count(ObjectData_Function const & f)
{
	switch (output_type)
	{
	case OUTPUT_ACSE:
		if (!f.external)
			string_offset += 4;
		break;

	default:
		throw SourceException("unknown output type for function_name", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_function_name_offset(std::ostream * out, ObjectData_Function const & f)
{
	switch (output_type)
	{
	case OUTPUT_ACSE:
		if (!f.external)
		{
			write_32(out, string_offset);
			string_offset += f.name.size() + 1;
		}
		break;

	default:
		throw SourceException("unknown output type for function_name_offset", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_registerarray(std::ostream * out, ObjectData_RegisterArray const & r)
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

void BinaryTokenZDACS::write_script(std::ostream * out, ObjectData_Script const & s)
{
	switch (output_type)
	{
	case OUTPUT_ACS0:
		write_32(out, (s.stype * 1000) + s.number);
		write_32(out, *ObjectExpression::get_symbol(s.label, SourcePosition::none));
		write_32(out, s.argCount <= 3 ? s.argCount : 3);
		break;

	case OUTPUT_ACSE:
		write_16(out, s.number);
		write_16(out, s.stype);
		write_32(out, *ObjectExpression::get_symbol(s.label, SourcePosition::none));
		write_32(out, s.argCount <= 3 ? s.argCount : 3);
		break;

	default:
		throw SourceException("unknown output type for script", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_script_flags(std::ostream * out, ObjectData_Script const & s)
{
	switch (output_type)
	{
	case OUTPUT_ACSE:
		if (!s.flags) break;
		write_16(out, s.number);
		write_16(out, s.flags);
		break;

	default:
		throw SourceException("unknown output type for script_flags", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_script_vars(std::ostream * out, ObjectData_Script const & s)
{
	switch (output_type)
	{
	case OUTPUT_ACSE:
		if (s.varCount <= 10) break;
		if (s.varCount <= 20 && target_type == TARGET_ZDoom) break;
		write_16(out, s.number);
		write_16(out, s.varCount);
		break;

	default:
		throw SourceException("unknown output type for script_vars", SourcePosition::none, "BinaryTokenZDACS");
	}
}

void BinaryTokenZDACS::write_string(std::ostream * out, ObjectData_String const & s)
{
	write_string(out, s.string);
}
void BinaryTokenZDACS::write_string(std::ostream * const out, std::string const & s)
{
	*out << s;
}
void BinaryTokenZDACS::write_string_offset(std::ostream * out, ObjectData_String const & s)
{
	switch (output_type)
	{
	case OUTPUT_ACS0:
		write_32(out, ObjectExpression::get_address_count() + 4 + (ObjectExpression::get_script_count() * 12) + 4 + (ObjectExpression::get_string_count() * 4) + s.offset);
		break;

	case OUTPUT_ACSE:
		write_32(out, 12 + (ObjectExpression::get_string_count() * 4) + s.offset);
		break;

	default:
		throw SourceException("unknown output type for string_offset", SourcePosition::none, "BinaryTokenZDACS");
	}
}


