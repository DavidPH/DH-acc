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

#include "SourcePosition.hpp"

#include <map>
#include <ostream>
#include <stdint.h>
#include <vector>



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
		OCODE_DELAYDIRECT,
		OCODE_DROP,
		OCODE_ENDPRINT,
		OCODE_LSPEC1,
		OCODE_LSPEC1DIRECT,
		OCODE_LSPEC2,
		OCODE_LSPEC2DIRECT,
		OCODE_LSPEC3,
		OCODE_LSPEC3DIRECT,
		OCODE_LSPEC4,
		OCODE_LSPEC4DIRECT,
		OCODE_LSPEC5,
		OCODE_LSPEC5DIRECT,
		OCODE_NOP,
		OCODE_PRINTCHARACTER,
		OCODE_PRINTNUMBER,
		OCODE_PRINTSTRING,
		OCODE_PUSHNUMBER,
		OCODE_PUSHSCRIPTVAR,
		OCODE_RESTART,
		OCODE_SUSPEND,
		OCODE_TERMINATE,

		// BinaryTokenZDACS
		OCODE_ASSIGNGLOBALARRAY,
		OCODE_DUP,
		OCODE_ENDLOG,
		OCODE_PRINTFIXED,
		OCODE_PUSHGLOBALARRAY,

		OCODE_NONE
	};



	ObjectToken(ObjectCode const code, SourcePosition const & position, std::vector<int32_t> const & args);

	int32_t getArgInt32(uintptr_t const index) const;

	ObjectCode getCode() const;

	SourcePosition const & getPosition() const;



	static void add_string(std::string const & symbol, std::string const & value);

	static void add_symbol(std::string const & symbol, int32_t const value);

	static std::string const & get_string(int32_t const index);

	static int32_t get_string_count();

	// Returns length of all strings combined.
	static int32_t get_string_length();

	static int32_t get_string_offset(int32_t const index);

	static int32_t get_symbol(std::string const & symbol, SourcePosition const & position);

private:
	std::vector<int32_t> _args;
	ObjectCode _code;
	SourcePosition _position;



	// {string, offset} Yes, signed. If it's a problem, I'll change it.
	static std::vector<std::pair<std::string, int32_t> > _string_table;
	static std::map<std::string, int32_t> _symbol_table;
};



#endif /* HPP_ObjectToken_ */



