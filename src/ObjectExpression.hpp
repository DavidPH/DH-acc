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

/* ObjectExpression.hpp
**
** ObjectExpression class.
*/

#ifndef HPP_ObjectExpression_
#define HPP_ObjectExpression_

#include "SourcePosition.hpp"

#include <map>
#include <ostream>
#include <stdint.h>
#include <string>
#include <vector>



class ObjectExpressionBase
{
public:
	ObjectExpressionBase(SourcePosition const & position);
	virtual ~ObjectExpressionBase();

	virtual ObjectExpressionBase * clone() const = 0;

	SourcePosition const & getPosition() const;

	virtual int32_t resolveInt32() const = 0;

public:
	SourcePosition _position;
};

class ObjectExpression
{
public:
	ObjectExpression();
	ObjectExpression(ObjectExpression const & expr);
	ObjectExpression(ObjectExpressionBase * const expr);
	~ObjectExpression();

	SourcePosition const & getPosition() const;

	ObjectExpression & operator = (ObjectExpression const & expr);

	int32_t resolveInt32() const;



	friend void print_debug(std::ostream * const out, ObjectExpression const & in);

	static void add_address_count(int32_t const addressCount);

	// Adds a label for the current address count.
	static void add_label(std::string const & symbol);

	// Adds a string using an auto-generated symbol and returns that symbol.
	static std::string add_string(std::string const & value);
	static void add_string(std::string const & symbol, std::string const & value);

	static void add_symbol(std::string const & symbol, ObjectExpression const & value);

	static ObjectExpression create_binary_add(ObjectExpression const & exprL, ObjectExpression const & exprR);
	static ObjectExpression create_binary_and(ObjectExpression const & exprL, ObjectExpression const & exprR);
	static ObjectExpression create_binary_div(ObjectExpression const & exprL, ObjectExpression const & exprR);
	static ObjectExpression create_binary_ior(ObjectExpression const & exprL, ObjectExpression const & exprR);
	static ObjectExpression create_binary_mod(ObjectExpression const & exprL, ObjectExpression const & exprR);
	static ObjectExpression create_binary_mul(ObjectExpression const & exprL, ObjectExpression const & exprR);
	static ObjectExpression create_binary_sub(ObjectExpression const & exprL, ObjectExpression const & exprR);
	static ObjectExpression create_binary_xor(ObjectExpression const & exprL, ObjectExpression const & exprR);

	static ObjectExpression create_unary_add(ObjectExpression const & expr);
	static ObjectExpression create_unary_sub(ObjectExpression const & expr);

	static ObjectExpression create_value_int32(int32_t const value, SourcePosition const & position);
	static ObjectExpression create_value_symbol(std::string const & symbol, SourcePosition const & position);

	static std::string const & get_string(int32_t const index);

	static int32_t get_string_count();

	// Returns length of all strings combined.
	static int32_t get_string_length();

	static int32_t get_string_offset(int32_t const index);

	static ObjectExpression get_symbol(std::string const & symbol, SourcePosition const & position);

private:
	ObjectExpressionBase * _expr;



	static int32_t _address_count;
	// {string, offset} Yes, signed. If it's a problem, I'll change it.
	static std::vector<std::pair<std::string, int32_t> > _string_table;
	static std::map<std::string, ObjectExpression> _symbol_table;
};



#endif /* HPP_ObjectExpression_ */



