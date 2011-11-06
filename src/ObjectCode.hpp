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

class SourcePosition;
class SourceTokenC;



enum ObjectCode
{
	// BinaryTokenACS
	OCODE_ACS_THING_COUNT,
	OCODE_ACS_THING_COUNT_IMM,
	OCODE_ACS_TIMER,
	OCODE_ADD,
	OCODE_ADDWORLDVAR,
	OCODE_AND,
	OCODE_ASSIGNMAPVAR,
	OCODE_ASSIGNSCRIPTVAR,
	OCODE_ASSIGNWORLDVAR,
	OCODE_BEGINPRINT,
	OCODE_BRANCH_IMM,
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
	OCODE_ZDACS_ACTIVATOR_TID,
	OCODE_ZDACS_GET_ACTOR_ANGLE,
	OCODE_ZDACS_GET_ACTOR_PITCH,
	OCODE_ZDACS_GET_ACTOR_PROPERTY,
	OCODE_ZDACS_GET_ACTOR_X,
	OCODE_ZDACS_GET_ACTOR_Y,
	OCODE_ZDACS_GET_ACTOR_Z,
	OCODE_ZDACS_GET_AMMO_CAPACITY,
	OCODE_ZDACS_INVENTORY_CHECK,
	OCODE_ZDACS_INVENTORY_CHECK_IMM,
	OCODE_ZDACS_INVENTORY_GIVE,
	OCODE_ZDACS_INVENTORY_GIVE_IMM,
	OCODE_ZDACS_INVENTORY_TAKE,
	OCODE_ZDACS_INVENTORY_TAKE_IMM,
	OCODE_ZDACS_PLAYER_NUMBER,
	OCODE_ZDACS_SET_ACTOR_ANGLE,
	OCODE_ZDACS_SET_ACTOR_PITCH,
	OCODE_ZDACS_SET_ACTOR_POSITION,
	OCODE_ZDACS_SET_ACTOR_PROPERTY,
	OCODE_ZDACS_SET_AMMO_CAPACITY,
	OCODE_ZDACS_SPAWN,
	OCODE_ZDACS_SPAWN_IMM,
	OCODE_ZDACS_SPAWN_PROJECTILE,

	OCODE_TRIG_COS,
	OCODE_TRIG_SIN,

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
	OCODE_BRANCHTABLE,
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

	/* s:addr */
	OCODE_BRANCH,

	/* a:offset s:addr->value */
	OCODE_GET_POINTER_VAR,

	/* a:offset s:addr,value */
	OCODE_SET_POINTER_VAR,

	OCODE_NONE
};

struct ObjectCodeSet
{
	ObjectCodeSet();

	ObjectCode ocode;
	ObjectCode ocode_imm;
};



ObjectCode ocode_get_code(SourceTokenC const & token);
ObjectCode ocode_get_code(std::string const & data, SourcePosition const & position);

// Returns true if the passed ocode pushes something to the stack without taking
// anything.
bool ocode_is_push_noarg(ObjectCode ocode);

char const * make_string(ObjectCode ocode);

void print_debug(std::ostream * out, ObjectCode in);
void print_debug(std::ostream * out, ObjectCodeSet const & in);

void read_object(std::istream * in, ObjectCode * out);
void read_object(std::istream * in, ObjectCodeSet * out);

void write_object(std::ostream * out, ObjectCode const & in);
void write_object(std::ostream * out, ObjectCodeSet const & in);



#endif /* HPP_ObjectCode_ */



