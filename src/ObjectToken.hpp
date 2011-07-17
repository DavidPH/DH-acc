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

#include <ostream>
#include <stdint.h>
#include <vector>

class ObjectExpression;



class ObjectToken
{
public:
	enum ObjectCode
	{
		// BinaryTokenACS
		OCODE_ADD,
		OCODE_ASSIGNSCRIPTVAR,
		OCODE_BEGINPRINT,
		OCODE_DELAY,
		OCODE_DELAY_IMM,
		OCODE_DIV,
		OCODE_DROP,
		OCODE_ENDPRINT,
		OCODE_GOTO,
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
		OCODE_PUSHNUMBER,
		OCODE_PUSHSCRIPTVAR,
		OCODE_RESTART,
		OCODE_SHIFTL,
		OCODE_SHIFTR,
		OCODE_SUB,
		OCODE_SUSPEND,
		OCODE_TERMINATE,

		// BinaryTokenZDACS
		OCODE_ASSIGNGLOBALARRAY,
		OCODE_DIVFIXED,
		OCODE_DUP,
		OCODE_ENDLOG,
		OCODE_LSPEC5RESULT,
		OCODE_MULFIXED,
		OCODE_PRINTFIXED,
		OCODE_PUSHGLOBALARRAY,

		OCODE_NONE
	};



	ObjectToken(ObjectCode const code, SourcePosition const & position);
	ObjectToken(ObjectCode const code, SourcePosition const & position, ObjectExpression const & arg);
	ObjectToken(ObjectCode const code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<ObjectExpression> const & args);

	void addLabel(std::string const & label);

	ObjectExpression const & getArg(uintptr_t const index) const;

	ObjectCode getCode() const;

	std::vector<std::string> const & getLabels() const;
	SourcePosition const & getPosition() const;



	friend void print_debug(std::ostream * const out, ObjectToken const & in);
	friend void print_debug(std::ostream * const out, ObjectToken::ObjectCode const in);

private:
	std::vector<ObjectExpression> _args;
	ObjectCode _code;
	std::vector<std::string> _labels;
	SourcePosition _position;
};



#endif /* HPP_ObjectToken_ */



