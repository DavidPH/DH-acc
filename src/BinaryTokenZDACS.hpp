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
		BCODE_LSPEC1DIRECT      =   9,
		BCODE_LSPEC2DIRECT      =  10,
		BCODE_LSPEC3DIRECT      =  11,
		BCODE_LSPEC4DIRECT      =  12,
		BCODE_LSPEC5DIRECT      =  13,
		BCODE_ADD               =  14,
		BCODE_ASSIGNSCRIPTVAR   =  25,
		BCODE_PUSHSCRIPTVAR     =  28,
		BCODE_DROP              =  54,
		BCODE_DELAY             =  55,
		BCODE_DELAYDIRECT       =  56,
		BCODE_RESTART           =  69,
		BCODE_BEGINPRINT        =  85,
		BCODE_ENDPRINT          =  86,
		BCODE_PRINTNUMBER       =  88,

		BCODE_PRINTFIXED        = 157,
		BCODE_DUP               = 216,
		BCODE_PUSHGLOBALARRAY   = 235,
		BCODE_ASSIGNGLOBALARRAY = 236,
		BCODE_ENDLOG            = 270,

		BCODE_NONE
	};



	BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position);
	BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, int32_t const arg0);
	BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, int32_t const arg0, int32_t const arg1);
	BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, int32_t const arg0, int32_t const arg1, int32_t const arg2);
	BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, int32_t const arg0, int32_t const arg1, int32_t const arg2, int32_t const arg3);
	BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, int32_t const arg0, int32_t const arg1, int32_t const arg2, int32_t const arg3, int32_t const arg4);
	BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, int32_t const arg0, int32_t const arg1, int32_t const arg2, int32_t const arg3, int32_t const arg4, int32_t const arg5);

	void write(std::ostream * const out) const;



	static void init();

	static void make_tokens(std::vector<ObjectToken> const & objects, std::vector<BinaryTokenZDACS> * const instructions);

private:
	int32_t _args[6];
	BinaryCode _code;
	SourcePosition _position;



	static void write_32(std::ostream * const out, uint32_t const i);

	static int _arg_counts[BCODE_NONE];
};



#endif /* HPP_BinaryTokenZDACS_ */



