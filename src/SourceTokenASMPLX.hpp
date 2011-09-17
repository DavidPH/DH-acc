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

/* SourceTokenASMPLX.hpp
**
** Defines the SourceTokenASMPLX class.
*/

#ifndef HPP_SourceTokenASMPLX_
#define HPP_SourceTokenASMPLX_

#include "bignum.hpp"
#include "CounterPointer.hpp"
#include "SourcePosition.hpp"

#include <string>
#include <vector>

class ObjectExpression;
class ObjectVector;
class SourceStream;



class SourceTokenASMPLX
{
public:
	typedef char TokenType;



	SourceTokenASMPLX();
	SourceTokenASMPLX(SourceStream * const in);



	static void make_objects(std::vector<SourceTokenASMPLX> const & tokens, ObjectVector * objects);

	static void read_tokens(SourceStream * const in, std::vector<SourceTokenASMPLX> * const tokens);

private:
	std::vector<std::string> _data;
	std::string _name;
	SourcePosition _position;
	TokenType _type;



	static bool isexprc(char const c);

	static bigsint char_to_int(char c, bigsint base, SourcePosition const & position);

	static CounterPointer<ObjectExpression> make_expression(std::string const & expr, SourcePosition const & position);

	// Returns -1 for number 0.
	static bigsint string_to_base(std::string const & s, SourcePosition const & position);
	static bigsint string_to_int(std::string const & s, SourcePosition const & position);
	static bigreal string_to_real(std::string const & s, SourcePosition const & position);
};



#endif /* HPP_SourceTokenASMPLX_ */



