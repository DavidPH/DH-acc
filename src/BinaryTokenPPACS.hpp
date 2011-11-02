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

/* BinaryTokenPPACS.hpp
**
** Defines the BinaryTokenPPACS class.
*/

#ifndef HPP_BinaryTokenPPACS_
#define HPP_BinaryTokenPPACS_

#include "bignum.hpp"
#include "CounterPointer.hpp"
#include "SourcePosition.hpp"

#include <ostream>
#include <vector>

struct ObjectData_Script;
struct ObjectData_String;
class ObjectExpression;
class ObjectToken;
class ObjectVector;



class BinaryTokenPPACS
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
		BCODE_CMPEQ             =  19,
		BCODE_CMPNE             =  20,
		BCODE_CMPLT             =  21,
		BCODE_CMPGT             =  22,
		BCODE_CMPLE             =  23,
		BCODE_CMPGE             =  24,
		BCODE_ASSIGNSCRIPTVAR   =  25,
		BCODE_ASSIGNMAPVAR      =  26,
		BCODE_ASSIGNWORLDVAR    =  27,
		BCODE_PUSHSCRIPTVAR     =  28,
		BCODE_PUSHMAPVAR        =  29,
		BCODE_PUSHWORLDVAR      =  30,
		BCODE_ADDWORLDVAR       =  33,
		BCODE_SUBWORLDVAR       =  36,
		BCODE_BRANCH            =  52,
		BCODE_BRANCHNOTZERO     =  53,
		BCODE_DROP              =  54,
		BCODE_DELAY             =  55,
		BCODE_DELAY_IMM         =  56,
		BCODE_RANDOM            =  57,
		BCODE_RANDOM_IMM        =  58,
		BCODE_THING_COUNT       =  59,
		BCODE_THING_COUNT_IMM   =  60,
		BCODE_RESTART           =  69,
		BCODE_LOGICALAND        =  70,
		BCODE_LOGICALIOR        =  71,
		BCODE_AND               =  72,
		BCODE_IOR               =  73,
		BCODE_XOR               =  74,
		BCODE_LOGICALNOT        =  75,
		BCODE_SHIFTL            =  76,
		BCODE_SHIFTR            =  77,
		BCODE_NEGATE            =  78,
		BCODE_BRANCHZERO        =  79,
		BCODE_BRANCHCASE        =  84,
		BCODE_BEGINPRINT        =  85,
		BCODE_ENDPRINT          =  86,
		BCODE_PRINTSTRING       =  87,
		BCODE_PRINTNUMBER       =  88,
		BCODE_PRINTCHARACTER    =  89,
		BCODE_TIMER             =  93,

		BCODE_NONE
	};



	BinaryTokenPPACS(BinaryCode code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<CounterPointer<ObjectExpression> > const & args);

	void addLabel(std::string const & label);

	size_t getArgCount() const;

	size_t size() const;

	void write(std::ostream * out) const;



	static void init();

	static void make_tokens(ObjectToken const & object, std::vector<BinaryTokenPPACS> * instructions);
	static void make_tokens(ObjectVector const & objects, std::vector<BinaryTokenPPACS> * instructions);

	static void write_all(std::ostream * out, std::vector<BinaryTokenPPACS> const & instructions);
	static void write_script(std::ostream * out, ObjectData_Script const & s);
	static void write_string(std::ostream * out, ObjectData_String const & s);
	static void write_string(std::ostream * out, std::string const & s);
	static void write_string_offset(std::ostream * out, ObjectData_String const & s);

private:
	std::vector<CounterPointer<ObjectExpression> > _args;
	BinaryCode _code;
	std::vector<std::string> _labels;
	SourcePosition _position;



	static int _arg_counts[BCODE_NONE];
};



#endif /* HPP_BinaryTokenPPACS_ */


