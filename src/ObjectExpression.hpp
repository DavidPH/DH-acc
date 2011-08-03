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

	struct Function
	{
		int_t argCount;
		std::string label;
		int_t retCount;
		int_t varCount;
	};

	struct Register
	{
		std::string name;
		int_t number;
		int_t size;
	};

	struct RegisterArray
	{
		std::string name;
		int_t number;
		int_t size;
	};

	struct Script
	{
		std::string label;
		std::string name;
		ScriptType stype;
		int_t argCount;
		int_t flags;
		int_t number;
		int_t varCount;
	};

	struct Static
	{
		std::string name;
		int_t number;
		int_t size;
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

	static void add_address_count(int32_t const addressCount);

	static void add_function(std::string const & label, int_t argCount, int_t varCount, int_t retCount);

	// Adds a label for the current address count.
	static void add_label(std::string const & symbol);

	static void add_register_global(std::string const & name, int_t size);
	static void add_register_global(std::string const & name, int_t size, int_t number);

	static void add_register_map(std::string const & name, int_t size);
	static void add_register_map(std::string const & name, int_t size, int_t number);

	static void add_register_world(std::string const & name, int_t size);
	static void add_register_world(std::string const & name, int_t size, int_t number);

	static void add_registerarray_global(std::string const & name, int_t size);
	static void add_registerarray_global(std::string const & name, int_t size, int_t number);

	static void add_registerarray_map(std::string const & name, int_t size);
	static void add_registerarray_map(std::string const & name, int_t size, int_t number);

	static void add_registerarray_world(std::string const & name, int_t size);
	static void add_registerarray_world(std::string const & name, int_t size, int_t number);

	static void add_script(std::string const & name, std::string const & label, ScriptType stype, int_t flags, int_t argCount, int_t varCount);
	static void add_script(std::string const & name, std::string const & label, ScriptType stype, int_t flags, int_t argCount, int_t varCount, int_t number);

	static void add_static(std::string const & name, int_t size);
	static void add_static(std::string const & name, int_t size, int_t number);

	// Adds a string using an auto-generated symbol and returns that symbol.
	static std::string add_string(std::string const & value);
	static void add_string(std::string const & symbol, std::string const & value);

	static void add_symbol(std::string const & symbol, ObjectExpression * value);
	static void add_symbol(std::string const & symbol, ExpressionType type);

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

	static void do_deferred_allocation();

	static float_t get_float(SourceTokenC const & token);

	static Function const & get_function(int_t index);

	static int_t get_function_count();

	static int_t get_int(SourceTokenC const & token);

	static RegisterArray const & get_registerarray_map(int_t index);

	static int_t get_registerarray_map_count();

	static ScriptFlag get_ScriptFlag(std::string const & value, SourcePosition const & position);

	static ScriptType get_ScriptType(std::string const & value, SourcePosition const & position);

	static Script const & get_script(int32_t const index);

	static int32_t get_script_count();

	static String const & get_string(int32_t const index);

	static int32_t get_string_count();

	// Returns length of all strings combined.
	static int32_t get_string_length();

	static ObjectExpression::Pointer get_symbol(std::string const & symbol, SourcePosition const & position);
	static ExpressionType get_symbol_type(std::string const & symbol, SourcePosition const & position);

	static void reserve_script_number(int32_t number);

	static void set_address_count(int32_t addressCount);

	static ObjectExpression::Pointer static_offset;

protected:
	ObjectExpression(SourcePosition const & position);

	SourcePosition position;

private:
	static void do_deferred_allocation_register(std::vector<Register> * registerTable, std::map<int_t, bool> * registerUsed);
	static void do_deferred_allocation_registerarray(std::vector<RegisterArray> * registerarrayTable, std::map<int_t, bool> * registerarrayUsed);

	static int_t get_register_number(std::map<int_t, bool> * registerUsed, int_t size);
	static int_t get_registerarray_number(std::map<int_t, bool> * registerarrayUsed);
	static int_t get_script_number();
	static int_t get_static_number(int_t size);

	static bool is_register_used(std::map<int_t, bool> * registerUsed, int_t number, int_t size);

	static int32_t _address_count;

	static std::vector<Function> _function_table;

	static std::vector<Register> _register_global_table;
	static std::map<int_t, bool> _register_global_used;
	static std::vector<Register> _register_map_table;
	static std::map<int_t, bool> _register_map_used;
	static std::vector<Register> _register_world_table;
	static std::map<int_t, bool> _register_world_used;

	static std::vector<RegisterArray> _registerarray_global_table;
	static std::map<int_t, bool>      _registerarray_global_used;
	static std::vector<RegisterArray> _registerarray_map_table;
	static std::map<int_t, bool>      _registerarray_map_used;
	static std::vector<RegisterArray> _registerarray_world_table;
	static std::map<int_t, bool>      _registerarray_world_used;

	static std::vector<Script>   _script_table;
	static std::map<int_t, bool> _script_used;

	static std::vector<Static>   _static_table;
	static std::map<int_t, bool> _static_used;

	static std::vector<String> _string_table;

	static std::map<std::string, ObjectExpression::Pointer> _symbol_table;
	static std::map<std::string, ExpressionType>            _symbol_type_table;
};



#endif /* HPP_ObjectExpression_ */



