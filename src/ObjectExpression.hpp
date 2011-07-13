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
	enum ScriptFlag
	{
		SF_NET        = 1,
		SF_CLIENTSIDE = 2
	};

	enum ScriptType
	{
		ST_CLOSED     =  0,
		ST_OPEN       =  1,
		ST_RESPAWN    =  2,
		ST_DEATH      =  3,
		ST_ENTER      =  4,
		ST_LIGHTNING  = 12,
		ST_UNLOADING  = 13,
		ST_DISCONNECT = 14,
		ST_RETURN     = 15
	};

	struct Script
	{
		int32_t args;
		int flags;
		std::string label;
		int32_t number;
		ScriptType type;
	};

	struct String
	{
		int32_t offset;
		std::string string;
	};



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

	static void add_script(std::string const & label, int32_t const number, ScriptType const type = ST_CLOSED, int32_t const args = 0, int const flags = 0);

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

	static ScriptFlag get_ScriptFlag(std::string const & value, SourcePosition const & position);

	static ScriptType get_ScriptType(std::string const & value, SourcePosition const & position);

	static Script const & get_script(int32_t const index);

	static int32_t get_script_count();

	static String const & get_string(int32_t const index);

	static int32_t get_string_count();

	// Returns length of all strings combined.
	static int32_t get_string_length();

	static ObjectExpression get_symbol(std::string const & symbol, SourcePosition const & position);

private:
	ObjectExpressionBase * _expr;



	static int32_t _address_count;
	static std::vector<Script> _script_table;
	static std::vector<String> _string_table;
	static std::map<std::string, ObjectExpression> _symbol_table;
};



#endif /* HPP_ObjectExpression_ */



