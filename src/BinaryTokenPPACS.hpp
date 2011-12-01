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

#include "BinaryTokenACS.hpp"
#include "bignum.hpp"
#include "CounterPointer.hpp"
#include "SourcePosition.hpp"

#include <ostream>
#include <vector>

struct ObjectData_Auto;
struct ObjectData_Script;
struct ObjectData_Static;
struct ObjectData_String;
class ObjectExpression;
class ObjectToken;
class ObjectVector;



class BinaryTokenPPACS
{
public:
	enum BinaryCode
	{
		BINTOKACS_BCODES,

		BCODE_SET_STATIC_VAR     = 102,
		BCODE_SET_AUTO_VAR       = 103,
		BCODE_SET_POINTER_VAR    = 104,
		BCODE_GET_STATIC_VAR     = 105,
		BCODE_GET_AUTO_VAR       = 106,
		BCODE_GET_POINTER_VAR    = 107,
		BCODE_ADDR_STACK_VAR     = 108,
		BCODE_ADDR_STACK_ADD     = 109,
		BCODE_ADDR_STACK_SUB     = 110,
		BCODE_BRANCH_GOTO        = 111,
		BCODE_SET_ACTIVATOR_MID  = 112,
		BCODE_GET_ACTIVATOR_MID  = 113,
		BCODE_SET_MOBJ_VAR       = 114,
		BCODE_GET_MOBJ_VAR       = 115,
		BCODE_MUL_FIXED          = 116,
		BCODE_DIV_FIXED          = 117,
		BCODE_PRINT_FIXED        = 118,
		BCODE_PRINT_END_LOG      = 119,
		BCODE_PRINT_END_ERROR    = 120,

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
	static void write_auto(std::ostream * out, ObjectData_Auto const & a);
	static void write_label(std::ostream * out, std::string const & label);
	static void write_script(std::ostream * out, ObjectData_Script const & s);
	static void write_static(std::ostream * out, ObjectData_Static const & s);
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


