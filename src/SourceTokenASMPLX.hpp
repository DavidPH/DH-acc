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

#include "ObjectToken.hpp"
#include "SourcePosition.hpp"

#include <map>
#include <stdint.h>
#include <string>
#include <vector>

class ObjectExpression;
class SourceStream;



class SourceTokenASMPLX
{
public:
	typedef char TokenType;



	SourceTokenASMPLX();
	SourceTokenASMPLX(SourceStream * const in);

	std::string const & getData(uintptr_t const index) const;

	uintptr_t getDataSize() const;

	std::string const & getName() const;

	SourcePosition const & getPosition() const;

	TokenType getType() const;



	static void init();

	static void make_objects(std::vector<SourceTokenASMPLX> const & tokens, std::vector<ObjectToken> * const objects);

	static void read_tokens(SourceStream * const in, std::vector<SourceTokenASMPLX> * const tokens);

private:
	std::vector<std::string> _data;
	std::string _name;
	SourcePosition _position;
	TokenType _type;



	static bool isexprc(char const c);

	static int32_t char_to_int(char const c, int32_t const base, SourcePosition const & position);

	static ObjectExpression make_expression(std::string const & expr, SourcePosition const & position);

	// Returns -1 for number 0.
	static int32_t string_to_base(std::string const & s, SourcePosition const & position);
	static int32_t string_to_int(std::string const & s, SourcePosition const & position);
	static long double string_to_real(std::string const & s, SourcePosition const & position);

	static std::map<std::string, std::pair<ObjectToken::ObjectCode, int> > _arg_counts;
};



#endif /* HPP_SourceTokenASMPLX_ */



