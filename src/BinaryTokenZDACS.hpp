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

/* BinaryTokenZDACS.hpp
**
** Defines the BinaryTokenZDACS class.
*/

#ifndef HPP_BinaryTokenZDACS_
#define HPP_BinaryTokenZDACS_

#include "BinaryTokenACS.hpp"
#include "bignum.hpp"
#include "CounterPointer.hpp"
#include "SourcePosition.hpp"

#include <ostream>
#include <vector>

struct ObjectData_Function;
struct ObjectData_RegisterArray;
struct ObjectData_Script;
struct ObjectData_String;
class ObjectExpression;
class ObjectToken;
class ObjectVector;



class BinaryTokenZDACS
{
public:
	enum BinaryCode
	{
		BINTOKACS_BCODES,

		BCODE_MUL_FIXED              = 136,
		BCODE_DIV_FIXED              = 137,
		BCODE_INVENTORY_GIVE         = 143,
		BCODE_INVENTORY_GIVE_IMM     = 144,
		BCODE_INVENTORY_TAKE         = 145,
		BCODE_INVENTORY_TAKE_IMM     = 146,
		BCODE_INVENTORY_CHECK        = 147,
		BCODE_INVENTORY_CHECK_IMM    = 148,
		BCODE_SPAWN                  = 149,
		BCODE_SPAWN_IMM              = 150,
		BCODE_PRINT_FIXED            = 157,
		BCODE_SET_GLOBALREGISTER_VAR = 181,
		BCODE_GET_GLOBALREGISTER_VAR = 182,
		BCODE_GET_TID_X              = 196,
		BCODE_GET_TID_Y              = 197,
		BCODE_GET_TID_Z              = 198,
		BCODE_CALLFUNC_IMM           = 203,
		BCODE_CALLFUNCVOID_IMM       = 204,
		BCODE_RETNFUNCVOID           = 205,
		BCODE_RETNFUNC               = 206,
		BCODE_GET_MAPARRAY_VAR       = 207,
		BCODE_SET_MAPARRAY_VAR       = 208,
		BCODE_STACK_DUP              = 216,
		BCODE_STACK_SWAP             = 217,
		BCODE_TRIG_SIN               = 220,
		BCODE_TRIG_COS               = 221,
		BCODE_GET_WORLDARRAY_VAR     = 226,
		BCODE_SET_WORLDARRAY_VAR     = 227,
		BCODE_GET_GLOBALARRAY_VAR    = 235,
		BCODE_SET_GLOBALARRAY_VAR    = 236,
		BCODE_SET_TID_VAR            = 245,
		BCODE_GET_TID_VAR            = 246,
		BCODE_PLAYERNUMBER           = 247,
		BCODE_GET_ACTIVATOR_TID      = 248,
		BCODE_MISC_STRLEN            = 253,
		BCODE_SCRIPT_SETRETURN       = 257,
		BCODE_GET_TID_ANGLE          = 260,
		BCODE_LINESPEC5RESULT        = 263,
		BCODE_PRINT_END_LOG          = 270,
		BCODE_GET_AMMO_CAPACITY      = 271,
		BCODE_SET_AMMO_CAPACITY      = 272,
		BCODE_SET_TID_ANGLE          = 276,
		BCODE_SPAWN_PROJECTILE       = 280,
		BCODE_SET_TID_POSITION       = 283,
		BCODE_BITWISE_NOT            = 330,
		BCODE_GET_TID_PITCH          = 331,
		BCODE_SET_TID_PITCH          = 332,
		BCODE_PRINT_NUM_HEX          = 350,
		BCODE_MISC_NATIVE            = 351,

		BCODE_BRANCH_TABLE,

		BCODE_NONE
	};



	BinaryTokenZDACS(BinaryCode code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<CounterPointer<ObjectExpression> > const & args);

	void addLabel(std::string const & label);

	size_t getArgCount() const;

	size_t size() const;

	void write(std::ostream * const out) const;



	static void init();

	static void make_tokens(ObjectToken const & object, std::vector<BinaryTokenZDACS> * instructions);
	static void make_tokens(ObjectVector const & objects, std::vector<BinaryTokenZDACS> * instructions);

	static void write_all(std::ostream * const out, std::vector<BinaryTokenZDACS> const & instructions);
	static void write_chunk(std::ostream * out, std::ostringstream * chunkout, char const * chunkname);
	static void write_function(std::ostream * out, ObjectData_Function const & f);
	static void write_function_name(std::ostream * out, ObjectData_Function const & f);
	static void write_function_name_count(ObjectData_Function const & f);
	static void write_function_name_offset(std::ostream * out, ObjectData_Function const & f);
	static void write_registerarray(std::ostream * out, ObjectData_RegisterArray const & r);
	static void write_script(std::ostream * out, ObjectData_Script const & s);
	static void write_script_flags(std::ostream * out, ObjectData_Script const & s);
	static void write_script_vars(std::ostream * out, ObjectData_Script const & s);
	static void write_string(std::ostream * out, ObjectData_String const & s);
	static void write_string(std::ostream * const out, std::string const & s);
	static void write_string_offset(std::ostream * out, ObjectData_String const & s);

private:
	std::vector<CounterPointer<ObjectExpression> > _args;
	BinaryCode _code;
	std::vector<std::string> _labels;
	SourcePosition _position;



	static int _arg_counts[BCODE_NONE];
};



#endif /* HPP_BinaryTokenZDACS_ */


