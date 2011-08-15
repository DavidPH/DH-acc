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
** Defines the ObjectExpression class.
*/

#ifndef HPP_ObjectExpression_
#define HPP_ObjectExpression_

#include "bignum.hpp"
#include "CounterPointer.hpp"
#include "option.hpp"
#include "SourcePosition.hpp"

#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

class ObjectVector;
class SourceTokenC;



class ObjectExpression : public Counter
{
	MAKE_ABSTRACT_COUNTER_CLASS_BASE(ObjectExpression, Counter);

public:
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
		ST_RETURN     = 15,

		ST_NONE
	};

	struct Function
	{
		bigsint argCount;
		std::string label;
		bigsint retCount;
		bigsint varCount;
	};

	struct Register
	{
		std::string name;
		bigsint number;
		bigsint size;
	};

	struct RegisterArray
	{
		std::string name;
		bigsint number;
		bigsint size;
	};

	struct Script
	{
		std::string label;
		std::string name;
		ScriptType stype;
		bigsint argCount;
		bigsint flags;
		bigsint number;
		bigsint varCount;
	};

	struct Static
	{
		std::string name;
		bigsint number;
		bigsint size;
	};

	struct String
	{
		std::string name;
		std::string string;
		bigsint offset;
	};



	SourcePosition const & getPosition() const;

	virtual ExpressionType getType() const = 0;

	virtual void printDebug(std::ostream * out) const;

	virtual bigreal resolveFloat() const;
	virtual bigsint resolveInt() const;



	friend void print_debug(std::ostream * out, ObjectExpression const & in);

	friend bool override_object(Function * out, Function const & in);
	friend bool override_object(Register * out, Register const & in);
	friend bool override_object(RegisterArray * out, RegisterArray const & in);
	friend bool override_object(Script * out, Script const & in);
	friend bool override_object(Static * out, Static const & in);
	friend bool override_object(String * out, String const & in);

	friend void read_object(std::istream * in, ExpressionType * out);
	friend void read_object(std::istream * in, Function * out);
	friend void read_object(std::istream * in, Pointer * out);
	friend void read_object(std::istream * in, Register * out);
	friend void read_object(std::istream * in, RegisterArray * out);
	friend void read_object(std::istream * in, Script * out);
	friend void read_object(std::istream * in, ScriptType * out);
	friend void read_object(std::istream * in, Static * out);
	friend void read_object(std::istream * in, String * out);

	friend void write_object(std::ostream * out, ExpressionType const & in);
	friend void write_object(std::ostream * out, Function const & in);
	friend void write_object(std::ostream * out, Pointer const & in);
	friend void write_object(std::ostream * out, Register const & in);
	friend void write_object(std::ostream * out, RegisterArray const & in);
	friend void write_object(std::ostream * out, Script const & in);
	friend void write_object(std::ostream * out, ScriptType const & in);
	friend void write_object(std::ostream * out, Static const & in);
	friend void write_object(std::ostream * out, String const & in);

	static void add_address_count(bigsint const addressCount);

	static void add_function(std::string const & label, bigsint argCount, bigsint varCount, bigsint retCount);

	// Adds a label for the current address count.
	static void add_label(std::string const & symbol);

	static void add_register_global(std::string const & name, bigsint size);
	static void add_register_global(std::string const & name, bigsint size, bigsint number);

	static void add_register_map(std::string const & name, bigsint size);
	static void add_register_map(std::string const & name, bigsint size, bigsint number);

	static void add_register_world(std::string const & name, bigsint size);
	static void add_register_world(std::string const & name, bigsint size, bigsint number);

	static void add_registerarray_global(std::string const & name, bigsint size);
	static void add_registerarray_global(std::string const & name, bigsint size, bigsint number);

	static void add_registerarray_map(std::string const & name, bigsint size);
	static void add_registerarray_map(std::string const & name, bigsint size, bigsint number);

	static void add_registerarray_world(std::string const & name, bigsint size);
	static void add_registerarray_world(std::string const & name, bigsint size, bigsint number);

	static void add_script(std::string const & name, std::string const & label, ScriptType stype, bigsint flags, bigsint argCount, bigsint varCount);
	static void add_script(std::string const & name, std::string const & label, ScriptType stype, bigsint flags, bigsint argCount, bigsint varCount, bigsint number);

	static void add_static(std::string const & name, bigsint size);
	static void add_static(std::string const & name, bigsint size, bigsint number);

	// Adds a string using an auto-generated name and returns that name.
	static std::string add_string(std::string const & value);
	static void add_string(std::string const & name, std::string const & value);

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

	static Pointer create_value_float(bigreal value, SourcePosition const & position);
	static Pointer create_value_int(bigsint value, SourcePosition const & position);
	static Pointer create_value_symbol(std::string const & symbol, SourcePosition const & position);

	static void do_deferred_allocation();

	static bigsint get_address_count();

	static std::string const & get_filename();

	static bigsint get_function_count();

	static ScriptFlag get_ScriptFlag(std::string const & value, SourcePosition const & position);

	static ScriptType get_ScriptType(std::string const & value, SourcePosition const & position);

	static bigsint get_script_count();

	static bigsint get_string_count();

	static ObjectExpression::Pointer get_symbol(std::string const & symbol, SourcePosition const & position);
	static ExpressionType get_symbol_type(std::string const & symbol, SourcePosition const & position);

	static void init();

	static void iter_function(void (*iter)(std::ostream *, Function const &), std::ostream * out);

	static void iter_registerarray_map(void (*iter)(std::ostream *, RegisterArray const &), std::ostream * out);

	static void iter_script(void (*iter)(std::ostream *, Script const &), std::ostream * out);

	static void iter_string(void (*iter)(std::ostream *, String const &), std::ostream * out);

	static void read_objects(std::istream * in, ObjectVector * objects);

	static void set_address_count(bigsint addressCount);

	// Sets the current filename, translating to make it a valid symbol.
	static void set_filename(std::string const & filename);

	static void write_objects(std::ostream * out, ObjectVector const & objects);

	static ObjectExpression::Pointer static_offset;

protected:
	enum ObjectType
	{
		OT_BINARY_ADD,
		OT_BINARY_AND,
		OT_BINARY_DIV,
		OT_BINARY_IOR,
		OT_BINARY_MOD,
		OT_BINARY_MUL,
		OT_BINARY_SUB,
		OT_BINARY_XOR,

		OT_UNARY_ADD,
		OT_UNARY_SUB,

		OT_VALUE_FLOAT,
		OT_VALUE_INT,
		OT_VALUE_SYMBOL,

		OT_NONE
	};

	ObjectExpression(SourcePosition const & position);
	ObjectExpression(std::istream * in);

	virtual void writeObject(std::ostream * out) const;

	SourcePosition position;



	friend void read_object(std::istream * in, ObjectType * out);

	friend void write_object(std::ostream * out, ObjectType const & in);

	static Pointer create_binary_add(std::istream * in);
	static Pointer create_binary_and(std::istream * in);
	static Pointer create_binary_div(std::istream * in);
	static Pointer create_binary_ior(std::istream * in);
	static Pointer create_binary_mod(std::istream * in);
	static Pointer create_binary_mul(std::istream * in);
	static Pointer create_binary_sub(std::istream * in);
	static Pointer create_binary_xor(std::istream * in);

	static Pointer create_unary_add(std::istream * in);
	static Pointer create_unary_sub(std::istream * in);

	static Pointer create_value_float(std::istream * in);
	static Pointer create_value_int(std::istream * in);
	static Pointer create_value_symbol(std::istream * in);

private:
	static void do_deferred_allocation_register(std::vector<Register> * registerTable, std::map<bigsint, bool> * registerUsed);
	static void do_deferred_allocation_registerarray(std::vector<RegisterArray> * registerarrayTable, std::map<bigsint, bool> * registerarrayUsed);

	static bool do_string_fold(size_t index);

	static bigsint get_register_number(std::map<bigsint, bool> * registerUsed, bigsint size);
	static bigsint get_registerarray_number(std::map<bigsint, bool> * registerarrayUsed);
	static bigsint get_script_number();
	static bigsint get_static_number(bigsint size);

	static bigsint get_string(std::string const & s);

	static bool is_register_used(std::map<bigsint, bool> * registerUsed, bigsint number, bigsint size);

	static bigsint _address_count;

	static std::string _filename;

	static std::vector<Function> _function_table;

	static std::vector<Register>   _register_global_table;
	static std::map<bigsint, bool> _register_global_used;
	static std::vector<Register>   _register_map_table;
	static std::map<bigsint, bool> _register_map_used;
	static std::vector<Register>   _register_world_table;
	static std::map<bigsint, bool> _register_world_used;

	static std::vector<RegisterArray> _registerarray_global_table;
	static std::map<bigsint, bool>    _registerarray_global_used;
	static std::vector<RegisterArray> _registerarray_map_table;
	static std::map<bigsint, bool>    _registerarray_map_used;
	static std::vector<RegisterArray> _registerarray_world_table;
	static std::map<bigsint, bool>    _registerarray_world_used;

	static std::vector<Script>     _script_table;
	static std::map<bigsint, bool> _script_used;

	static std::vector<Static>     _static_table;
	static std::map<bigsint, bool> _static_used;

	static std::vector<String> _string_table;

	static std::map<std::string, ObjectExpression::Pointer> _symbol_table;
	static std::map<std::string, ExpressionType>            _symbol_type_table;
};



extern option::option_b option_string_fold;



#endif /* HPP_ObjectExpression_ */



