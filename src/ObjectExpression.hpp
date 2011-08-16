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
#include "ObjectData.hpp"
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



	SourcePosition const & getPosition() const;

	virtual ExpressionType getType() const = 0;

	virtual void printDebug(std::ostream * out) const;

	virtual bigreal resolveFloat() const;
	virtual bigsint resolveInt() const;



	friend void print_debug(std::ostream * out, ObjectExpression const & in);

	friend void read_object(std::istream * in, ExpressionType * out);
	friend void read_object(std::istream * in, Pointer * out);

	friend void write_object(std::ostream * out, ExpressionType const & in);
	friend void write_object(std::ostream * out, Pointer const & in);

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

	static void add_script(std::string const & name, std::string const & label, ObjectData_Script::ScriptType stype, bigsint flags, bigsint argCount, bigsint varCount);
	static void add_script(std::string const & name, std::string const & label, ObjectData_Script::ScriptType stype, bigsint flags, bigsint argCount, bigsint varCount, bigsint number);

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

	static bigsint get_script_count();

	static bigsint get_string_count();

	static ObjectExpression::Pointer get_symbol(std::string const & symbol, SourcePosition const & position);
	static ExpressionType get_symbol_type(std::string const & symbol, SourcePosition const & position);

	static void init();

	static void iter_function(void (*iter)(std::ostream *, ObjectData_Function const &), std::ostream * out);

	static void iter_registerarray_map(void (*iter)(std::ostream *, ObjectData_RegisterArray const &), std::ostream * out);

	static void iter_script(void (*iter)(std::ostream *, ObjectData_Script const &), std::ostream * out);

	static void iter_string(void (*iter)(std::ostream *, ObjectData_String const &), std::ostream * out);

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
	static void do_deferred_allocation_register(std::map<std::string, ObjectData_Register> * registerTable, std::map<bigsint, bool> * registerUsed);
	static void do_deferred_allocation_registerarray(std::map<std::string, ObjectData_RegisterArray> * registerarrayTable, std::map<bigsint, bool> * registerarrayUsed);

	static bool do_string_fold(size_t index);

	static bigsint get_register_number(std::map<bigsint, bool> * registerUsed, bigsint size);
	static bigsint get_registerarray_number(std::map<bigsint, bool> * registerarrayUsed);
	static bigsint get_script_number();
	static bigsint get_static_number(bigsint size);

	static bigsint get_string(std::string const & s);

	static bool is_register_used(std::map<bigsint, bool> * registerUsed, bigsint number, bigsint size);

	static bigsint _address_count;

	static std::string _filename;

	static std::vector<ObjectData_Function> _function_table;

	static std::map<std::string, ObjectData_Register> _register_global_table;
	static std::map<bigsint, bool>                    _register_global_used;
	static std::map<std::string, ObjectData_Register> _register_map_table;
	static std::map<bigsint, bool>                    _register_map_used;
	static std::map<std::string, ObjectData_Register> _register_world_table;
	static std::map<bigsint, bool>                    _register_world_used;

	static std::map<std::string, ObjectData_RegisterArray> _registerarray_global_table;
	static std::map<bigsint, bool>                         _registerarray_global_used;
	static std::map<std::string, ObjectData_RegisterArray> _registerarray_map_table;
	static std::map<bigsint, bool>                         _registerarray_map_used;
	static std::map<std::string, ObjectData_RegisterArray> _registerarray_world_table;
	static std::map<bigsint, bool>                         _registerarray_world_used;

	static std::map<std::string, ObjectData_Script> _script_table;
	static std::map<bigsint, bool>                  _script_used;

	static std::map<std::string, ObjectData_Static> _static_table;
	static std::map<bigsint, bool>                  _static_used;

	static std::vector<ObjectData_String> _string_table;

	static std::map<std::string, ObjectExpression::Pointer> _symbol_table;
	static std::map<std::string, ExpressionType>            _symbol_type_table;
};



extern option::option_b option_string_fold;



#endif /* HPP_ObjectExpression_ */



