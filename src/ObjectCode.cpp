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

/* ObjectCode.cpp
**
** Defines the ObjectCode related function.
*/

#include "ObjectCode.hpp"

#include "SourceException.hpp"
#include "SourceTokenC.hpp"

#include <map>
#include <string>



static std::map<std::string, ObjectCode> _ocode_map;
static char const * _ocode_str[OCODE_NONE+1];

static struct _ocode_init_s {_ocode_init_s();} _ocode_init;

_ocode_init_s::_ocode_init_s()
{
	#define DO_INIT(NAME,ARGC)\
	_ocode_map[#NAME] = OCODE_##NAME;\
	_ocode_str[OCODE_##NAME] = "OCODE_"#NAME

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

	// ObjectToken
	DO_INIT(ADDSTACK_IMM,       1);
	DO_INIT(ASSIGNGLOBALARRAY2, 2);
	DO_INIT(ASSIGNMAPARRAY2,    2);
	DO_INIT(ASSIGNSTACKARRAY2,  2);
	DO_INIT(ASSIGNSTACKVAR,     1);
	DO_INIT(ASSIGNSTATICARRAY2, 2);
	DO_INIT(ASSIGNSTATICVAR,    1);
	DO_INIT(ASSIGNWORLDARRAY2,  2);
	DO_INIT(LOGICALXOR,         0);
	DO_INIT(PUSHGLOBALARRAY2,   2);
	DO_INIT(PUSHMAPARRAY2,      2);
	DO_INIT(PUSHSTACKADDRESS,   1);
	DO_INIT(PUSHSTACKARRAY2,    2);
	DO_INIT(PUSHSTACKVAR,       1);
	DO_INIT(PUSHSTATICARRAY2,   2);
	DO_INIT(PUSHSTATICVAR,      1);
	DO_INIT(PUSHWORLDARRAY2,    2);
	DO_INIT(SUBSTACK_IMM,       1);

	DO_INIT(NONE, 0);

	#undef DO_INIT
}

ObjectCode ocode_get_code(SourceTokenC const & token)
{
	std::map<std::string, ObjectCode>::iterator codeIt(_ocode_map.find(token.getData()));

	if (codeIt == _ocode_map.end())
		throw SourceException("no such OCODE '" + token.getData() + "'", token.getPosition(), "ObjectToken");

	return codeIt->second;
}

bool ocode_is_push(ObjectCode ocode)
{
	switch (ocode)
	{
	// ADDRESS.
	case OCODE_PUSHSTACKADDRESS:
	// ARRAY.
	case OCODE_PUSHGLOBALARRAY2:
	case OCODE_PUSHMAPARRAY2:
	case OCODE_PUSHSTACKARRAY2:
	case OCODE_PUSHSTATICARRAY2:
	case OCODE_PUSHWORLDARRAY2:
	// LITERAL.
	case OCODE_PUSHNUMBER:
	// VAR.
	case OCODE_PUSHGLOBALVAR:
	case OCODE_PUSHMAPVAR:
	case OCODE_PUSHSCRIPTVAR:
	case OCODE_PUSHWORLDVAR:
	case OCODE_PUSHSTACKVAR:
	case OCODE_PUSHSTATICVAR:
		return true;

	default:
		return false;
	}
}

char const * make_string(ObjectCode ocode)
{
	if (ocode <= OCODE_NONE)
		return _ocode_str[ocode];
	else
		return "OCODE";
}

void print_debug(std::ostream * out, ObjectCode in)
{
	if (in <= OCODE_NONE)
		*out << _ocode_str[in];
	else
		*out << "OCODE";
}



