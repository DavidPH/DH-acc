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

/* ObjectToken.hpp
**
** ObjectToken class.
*/

#ifndef HPP_ObjectToken_
#define HPP_ObjectToken_

#include "ObjectExpression.hpp"
#include "SourcePosition.hpp"

#include <map>
#include <ostream>
#include <stdint.h>
#include <string>
#include <vector>

class ObjectExpression;
class SourceTokenC;



class ObjectToken
{
public:
	enum ObjectCode
	{
		// BinaryTokenACS
		OCODE_ADD,
		OCODE_ASSIGNMAPVAR,
		OCODE_ASSIGNSCRIPTVAR,
		OCODE_ASSIGNWORLDVAR,
		OCODE_BEGINPRINT,
		OCODE_BRANCH,
		OCODE_BRANCHNOTZERO,
		OCODE_BRANCHZERO,
		OCODE_DELAY,
		OCODE_DELAY_IMM,
		OCODE_DIV,
		OCODE_DROP,
		OCODE_ENDPRINT,
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
		OCODE_SUSPEND,
		OCODE_TERMINATE,

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
		OCODE_PRINTFIXED,
		OCODE_PUSHGLOBALARRAY,
		OCODE_PUSHGLOBALVAR,
		OCODE_PUSHMAPARRAY,
		OCODE_PUSHWORLDARRAY,
		OCODE_RETURNZDACS,
		OCODE_RETURNZDACSVOID,
		OCODE_SETRESULTVALUE,
		OCODE_STRLEN,
		OCODE_SWAP,

		OCODE_NONE
	};



	ObjectToken(ObjectCode const code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<ObjectExpression::Pointer> const & args);

	void addLabel(std::string const & label);

	ObjectExpression::Pointer getArg(size_t index) const;

	ObjectCode getCode() const;

	std::vector<std::string> const & getLabels() const;
	SourcePosition const & getPosition() const;



	friend void print_debug(std::ostream * const out, ObjectToken const & in);
	friend void print_debug(std::ostream * const out, ObjectToken::ObjectCode const in);

	static ObjectCode get_code(SourceTokenC const & token);

	static void init();

private:
	std::vector<ObjectExpression::Pointer> _args;
	ObjectCode _code;
	std::vector<std::string> _labels;
	SourcePosition _position;



	static std::map<std::string, ObjectCode> _codes;
};



#endif /* HPP_ObjectToken_ */



