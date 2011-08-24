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

/* ObjectCode.hpp
**
** Defines the ObjectCode enum and related functions.
*/

#ifndef HPP_ObjectCode_
#define HPP_ObjectCode_

#include <ostream>

class SourceTokenC;



enum ObjectCode
{
	// BinaryTokenACS
	OCODE_ADD,
	OCODE_ADDWORLDVAR,
	OCODE_AND,
	OCODE_ASSIGNMAPVAR,
	OCODE_ASSIGNSCRIPTVAR,
	OCODE_ASSIGNWORLDVAR,
	OCODE_BEGINPRINT,
	OCODE_BRANCH,
	OCODE_BRANCHCASE,
	OCODE_BRANCHNOTZERO,
	OCODE_BRANCHZERO,
	OCODE_CMPEQ,
	OCODE_CMPGE,
	OCODE_CMPGT,
	OCODE_CMPLE,
	OCODE_CMPLT,
	OCODE_CMPNE,
	OCODE_DELAY,
	OCODE_DELAY_IMM,
	OCODE_DIV,
	OCODE_DROP,
	OCODE_ENDPRINT,
	OCODE_IOR,
	OCODE_LOGICALAND,
	OCODE_LOGICALIOR,
	OCODE_LOGICALNOT,
	OCODE_LSPEC1,
	OCODE_LSPEC1_IMM,
	OCODE_LSPEC2,
	OCODE_LSPEC2_IMM,
	OCODE_LSPEC3,
	OCODE_LSPEC3_IMM,
	OCODE_LSPEC4,
	OCODE_LSPEC4_IMM,
	OCODE_LSPEC5,
	OCODE_LSPEC5_IMM,
	OCODE_MUL,
	OCODE_MOD,
	OCODE_NEGATE,
	OCODE_NOP,
	OCODE_PRINTCHARACTER,
	OCODE_PRINTNUMBER,
	OCODE_PRINTSTRING,
	OCODE_PUSHMAPVAR,
	OCODE_PUSHNUMBER,
	OCODE_PUSHSCRIPTVAR,
	OCODE_PUSHWORLDVAR,
	OCODE_RANDOM,
	OCODE_RANDOM_IMM,
	OCODE_RESTART,
	OCODE_SHIFTL,
	OCODE_SHIFTR,
	OCODE_SUB,
	OCODE_SUBWORLDVAR,
	OCODE_SUSPEND,
	OCODE_TERMINATE,
	OCODE_XOR,

	// BinaryTokenZDACS
	OCODE_ASSIGNGLOBALARRAY,
	OCODE_ASSIGNGLOBALVAR,
	OCODE_ASSIGNMAPARRAY,
	OCODE_ASSIGNWORLDARRAY,
	OCODE_CALLZDACS,
	OCODE_CALLZDACSDISCARD,
	OCODE_CALLZDFUNC,
	OCODE_DIVFIXED,
	OCODE_DUP,
	OCODE_ENDLOG,
	OCODE_LSPEC5RESULT,
	OCODE_MULFIXED,
	OCODE_NOT,
	OCODE_PRINTFIXED,
	OCODE_PRINTHEX,
	OCODE_PUSHGLOBALARRAY,
	OCODE_PUSHGLOBALVAR,
	OCODE_PUSHMAPARRAY,
	OCODE_PUSHWORLDARRAY,
	OCODE_RETURNZDACS,
	OCODE_RETURNZDACSVOID,
	OCODE_SETRESULTVALUE,
	OCODE_STRLEN,
	OCODE_SWAP,

	// ObjectToken
	OCODE_ADDSTACK_IMM,
	OCODE_ASSIGNGLOBALARRAY2,
	OCODE_ASSIGNMAPARRAY2,
	OCODE_ASSIGNPOINTER,
	OCODE_ASSIGNSTACKARRAY2,
	OCODE_ASSIGNSTACKVAR,
	OCODE_ASSIGNSTATICARRAY2,
	OCODE_ASSIGNSTATICVAR,
	OCODE_ASSIGNWORLDARRAY2,
	OCODE_LOGICALXOR,
	OCODE_PUSHGLOBALARRAY2,
	OCODE_PUSHMAPARRAY2,
	OCODE_PUSHPOINTER,
	OCODE_PUSHSTACKADDRESS,
	OCODE_PUSHSTACKARRAY2,
	OCODE_PUSHSTACKVAR,
	OCODE_PUSHSTATICARRAY2,
	OCODE_PUSHSTATICVAR,
	OCODE_PUSHWORLDARRAY2,
	OCODE_SUBSTACK_IMM,

	OCODE_NONE
};

struct ObjectCodeSet
{
	ObjectCodeSet();

	ObjectCode ocode;
	ObjectCode ocode_imm;
};



ObjectCode ocode_get_code(SourceTokenC const & token);

bool ocode_is_push(ObjectCode ocode);

char const * make_string(ObjectCode ocode);

void print_debug(std::ostream * out, ObjectCode in);
void print_debug(std::ostream * out, ObjectCodeSet const & in);

void read_object(std::istream * in, ObjectCode * out);
void read_object(std::istream * in, ObjectCodeSet * out);

void write_object(std::ostream * out, ObjectCode const & in);
void write_object(std::ostream * out, ObjectCodeSet const & in);



#endif /* HPP_ObjectCode_ */



