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

#include "CounterPointer.hpp"
#include "SourcePosition.hpp"

#include <map>
#include <ostream>
#include <stdint.h>
#include <string>
#include <vector>

class SourceTokenC;



class ObjectExpression : public Counter
{
	MAKE_ABSTRACT_COUNTER_CLASS_BASE(ObjectExpression, Counter);

public:
	typedef long double float_t;
	typedef long int int_t;

	enum ExpressionType
	{
		ET_FLOAT,
		ET_INT
	};

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

	struct ACSFunc
	{
		int_t argCount;
		std::string label;
		int_t retCount;
		int_t varCount;
	};

	struct RegisterArray
	{
		std::string name;
		int_t number;
		int_t size;
	};

	struct Script
	{
		int32_t args;
		int flags;
		std::string label;
		int32_t number;
		ScriptType type;
		int vars;
	};

	struct String
	{
		int32_t offset;
		std::string string;
	};



	SourcePosition const & getPosition() const;

	virtual ExpressionType getType() const = 0;

	virtual void printDebug(std::ostream * out) const;

	virtual float_t resolveFloat() const;
	virtual int_t resolveInt() const;



	friend void print_debug(std::ostream * out, ObjectExpression const & in);

	static void add_acsfunc(std::string const & label, int_t argCount, int_t varCount, int_t retCount);

	static void add_address_count(int32_t const addressCount);

	// Adds a label for the current address count.
	static void add_label(std::string const & symbol);

	static void add_registerarray_map(std::string const & name, int_t number, int_t size);

	static void add_script(std::string const & label, int32_t number, ScriptType type, int32_t args, int vars, int flags);

	// Adds a string using an auto-generated symbol and returns that symbol.
	static std::string add_string(std::string const & value);
	static void add_string(std::string const & symbol, std::string const & value);

	static void add_symbol(std::string const & symbol, ObjectExpression * value);

	static Pointer create_binary_add(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_and(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_div(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_ior(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_mod(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_mul(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_sub(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_xor(ObjectExpression * exprL, ObjectExpression * exprR, SourcePosition const & position);

	static Pointer create_unary_add(ObjectExpression * expr, SourcePosition const & position);
	static Pointer create_unary_sub(ObjectExpression * expr, SourcePosition const & position);

	static Pointer create_value_float(float_t value, SourcePosition const & position);
	static Pointer create_value_float(SourceTokenC const & token);
	static Pointer create_value_int(int_t value, SourcePosition const & position);
	static Pointer create_value_int(SourceTokenC const & token);
	static Pointer create_value_symbol(std::string const & symbol, SourcePosition const & position);

	static ACSFunc const & get_acsfunc(int_t const index);

	static int_t get_acsfunc_count();

	static float_t get_float(SourceTokenC const & token);

	static int_t get_int(SourceTokenC const & token);

	static RegisterArray const & get_registerarray_map(int_t index);

	static int_t get_registerarray_map_count();

	static ScriptFlag get_ScriptFlag(std::string const & value, SourcePosition const & position);

	static ScriptType get_ScriptType(std::string const & value, SourcePosition const & position);

	static Script const & get_script(int32_t const index);

	static int32_t get_script_count();

	static int32_t get_script_number();

	static String const & get_string(int32_t const index);

	static int32_t get_string_count();

	// Returns length of all strings combined.
	static int32_t get_string_length();

	static Pointer get_symbol(std::string const & symbol, SourcePosition const & position);

	static void reserve_script_number(int32_t number);

	static void set_address_count(int32_t addressCount);

protected:
	ObjectExpression(SourcePosition const & position);

	SourcePosition position;



private:
	static std::vector<ACSFunc> _acsfunc_table;
	static int32_t _address_count;
	static std::vector<RegisterArray> _registerarray_map_table;
	static std::vector<Script> _script_table;
	static std::map<int32_t, bool> _script_used;
	static int32_t _script_used_last;
	static std::vector<String> _string_table;
	static std::map<std::string, ObjectExpression::Pointer> _symbol_table;
};



#endif /* HPP_ObjectExpression_ */



