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
** BinaryTokenZDACS class.
*/

#ifndef HPP_BinaryTokenZDACS_
#define HPP_BinaryTokenZDACS_

#include "ObjectExpression.hpp"
#include "SourcePosition.hpp"

#include <ostream>
#include <stdint.h>
#include <vector>

class ObjectToken;



class BinaryTokenZDACS
{
public:
	enum BinaryCode
	{
		BCODE_NOP               =   0,
		BCODE_TERMINATE         =   1,
		BCODE_SUSPEND           =   2,
		BCODE_PUSHNUMBER        =   3,
		BCODE_LSPEC1            =   4,
		BCODE_LSPEC2            =   5,
		BCODE_LSPEC3            =   6,
		BCODE_LSPEC4            =   7,
		BCODE_LSPEC5            =   8,
		BCODE_LSPEC1_IMM        =   9,
		BCODE_LSPEC2_IMM        =  10,
		BCODE_LSPEC3_IMM        =  11,
		BCODE_LSPEC4_IMM        =  12,
		BCODE_LSPEC5_IMM        =  13,
		BCODE_ADD               =  14,
		BCODE_SUB               =  15,
		BCODE_MUL               =  16,
		BCODE_DIV               =  17,
		BCODE_MOD               =  18,
		BCODE_ASSIGNSCRIPTVAR   =  25,
		BCODE_PUSHSCRIPTVAR     =  28,
		BCODE_GOTO              =  52,
		BCODE_DROP              =  54,
		BCODE_DELAY             =  55,
		BCODE_DELAY_IMM         =  56,
		BCODE_RESTART           =  69,
		BCODE_SHIFTL            =  76,
		BCODE_SHIFTR            =  77,
		BCODE_BEGINPRINT        =  85,
		BCODE_ENDPRINT          =  86,
		BCODE_PRINTSTRING       =  87,
		BCODE_PRINTNUMBER       =  88,
		BCODE_PRINTCHARACTER    =  89,

		BCODE_MULFIXED          = 136,
		BCODE_DIVFIXED          = 137,
		BCODE_PRINTFIXED        = 157,
		BCODE_DUP               = 216,
		BCODE_PUSHGLOBALARRAY   = 235,
		BCODE_ASSIGNGLOBALARRAY = 236,
		BCODE_SETRESULTVALUE    = 257,
		BCODE_LSPEC5RESULT      = 263,
		BCODE_ENDLOG            = 270,

		BCODE_NONE
	};



	BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<ObjectExpression> const & args);

	void addLabel(std::string const & label);

	void write(std::ostream * const out) const;



	static void init();

	static void make_tokens(std::vector<ObjectToken> const & objects, std::vector<BinaryTokenZDACS> * const instructions);

	static void write_16(std::ostream * const out, uint16_t const i);
	static void write_32(std::ostream * const out, uint32_t const i);
	static void write_all(std::ostream * const out, std::vector<BinaryTokenZDACS> const & instructions);
	static void write_script(std::ostream * const out, ObjectExpression::Script const & s);
	static void write_script_flags(std::ostream * const out, ObjectExpression::Script const & s);
	static void write_script_vars(std::ostream * const out, ObjectExpression::Script const & s);
	static void write_string(std::ostream * const out, std::string const & s);

private:
	std::vector<ObjectExpression> _args;
	BinaryCode _code;
	std::vector<std::string> _labels;
	SourcePosition _position;



	static uintptr_t _arg_counts[BCODE_NONE];
};



#endif /* HPP_BinaryTokenZDACS_ */



