//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
// Base class for object-level expression handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_ObjectExpression_
#define HPP_ObjectExpression_

#include "bignum.hpp"
#include "Counter.hpp"
#include "ObjectData.hpp"
#include "SourcePosition.hpp"

#include <istream>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>

struct ObjectCodeSet;
class ObjectVector;
class SourceTokenC;


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression
//
class ObjectExpression : public Counter
{
	MAKE_ABSTRACT_COUNTER_CLASS_BASE(ObjectExpression, Counter);

public:
	enum ExpressionType
	{
		ET_OCODE,
		ET_FLOAT,
		ET_INT
	};


   virtual bool canResolve() const = 0;

	SourcePosition const & getPosition() const;

	virtual ExpressionType getType() const = 0;

	virtual bigreal resolveFloat() const;
	virtual bigsint resolveInt() const;
	virtual ObjectCodeSet resolveOCode() const;

   void writeACSP(std::ostream *out) const;


	friend bool override_object(ObjectExpression::ExpressionType * out, ObjectExpression::ExpressionType const & in);
	friend bool override_object(ObjectExpression::Pointer * out, ObjectExpression::Pointer const & in);

	friend void read_object(std::istream * in, ExpressionType * out);
	friend void read_object(std::istream * in, Pointer * out);

	friend void write_object(std::ostream * out, ExpressionType const & in);
	friend void write_object(std::ostream * out, Pointer const & in);

	static void add_address_count(bigsint const addressCount);

	static void add_auto(std::string const & name, bigsint size, bigsint number);

	static void add_function(std::string const & name, std::string const & label, bigsint argCount, bigsint varCount, bigsint retCount, bool externDef);

	// Adds a label for the current address count.
	static void add_label(std::string const & symbol);

	static void add_register_global(std::string const & name, bigsint size, bool externDef, bool externVis);
	static void add_register_global(std::string const & name, bigsint size, bool externDef, bool externVis, bigsint number);

	static void add_register_map(std::string const & name, bigsint size, bool externDef, bool externVis);
	static void add_register_map(std::string const & name, bigsint size, bool externDef, bool externVis, bigsint number);

	static void add_register_world(std::string const & name, bigsint size, bool externDef, bool externVis);
	static void add_register_world(std::string const & name, bigsint size, bool externDef, bool externVis, bigsint number);

	static void add_registerarray_global(std::string const & name, bigsint size, bool externDef, bool externVis);
	static void add_registerarray_global(std::string const & name, bigsint size, bool externDef, bool externVis, bigsint number);

	static void add_registerarray_map(std::string const & name, bigsint size, bool externDef, bool externVis);
	static void add_registerarray_map(std::string const & name, bigsint size, bool externDef, bool externVis, bigsint number);

	static void add_registerarray_world(std::string const & name, bigsint size, bool externDef, bool externVis);
	static void add_registerarray_world(std::string const & name, bigsint size, bool externDef, bool externVis, bigsint number);

	static void add_script(std::string const & name, std::string const & label, ObjectData_Script::ScriptType stype, bigsint flags, bigsint argCount, bigsint varCount);
	static void add_script(std::string const & name, std::string const & label, ObjectData_Script::ScriptType stype, bigsint flags, bigsint argCount, bigsint varCount, bigsint number);

	static void add_static(std::string const & name, bigsint size);
	static void add_static(std::string const & name, bigsint size, bigsint number);

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

   static Pointer create_branch_and(ObjectExpression *exprL, ObjectExpression *exprR, SourcePosition const &position);
   static Pointer create_branch_ior(ObjectExpression *exprL, ObjectExpression *exprR, SourcePosition const &position);
   static Pointer create_branch_xor(ObjectExpression *exprL, ObjectExpression *exprR, SourcePosition const &position);

   static Pointer create_cast_float_to_int(ObjectExpression *expr, SourcePosition const &position);
   static Pointer create_cast_int_to_float(ObjectExpression *expr, SourcePosition const &position);

	static Pointer create_unary_add(ObjectExpression * expr, SourcePosition const & position);
   static Pointer create_unary_lognot(ObjectExpression *expr, SourcePosition const &position);
	static Pointer create_unary_sub(ObjectExpression * expr, SourcePosition const & position);

	static Pointer create_value_ocode(ObjectCodeSet const & ocode, SourcePosition const & position);
	static Pointer create_value_float(bigreal value, SourcePosition const & position);
	static Pointer create_value_int(bigsint value, SourcePosition const & position);
	static Pointer create_value_symbol(std::string const & symbol, SourcePosition const & position);

	static void do_deferred_allocation();

	static bigsint get_address_count();

	static std::string const & get_filename();

	static ObjectExpression::Pointer get_symbol(std::string const & symbol, SourcePosition const & position);
   static ObjectExpression::Pointer get_symbol_null(std::string const &symbol);
	static ExpressionType get_symbol_type(std::string const & symbol, SourcePosition const & position);

	static void iter_auto(void (*iter)(std::ostream *, ObjectData_Auto const &), std::ostream * out);

	static void iter_function(void (*iter)(std::ostream *, ObjectData_Function const &), std::ostream * out);

	static void iter_library(void (*iter)(std::ostream *, std::string const &), std::ostream * out);

	static void iter_register_map(void (*iter)(std::ostream *, ObjectData_Register const &), std::ostream * out);

	static void iter_registerarray_map(void (*iter)(std::ostream *, ObjectData_RegisterArray const &), std::ostream * out);

	static void iter_script(void (*iter)(std::ostream *, ObjectData_Script const &), std::ostream * out);

	static void iter_static(void (*iter)(std::ostream *, ObjectData_Static const &), std::ostream * out);

	static void read_objects(std::istream * in, ObjectVector * objects);

	static void set_address_count(bigsint addressCount);

	// Sets the current filename, translating to make it a valid symbol.
	static void set_filename(std::string const & filename);

	static void set_library(std::string const & library);

	static void write_objects(std::ostream * out, ObjectVector const & objects);

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

      OT_BRANCH_AND,
      OT_BRANCH_IOR,
      OT_BRANCH_XOR,

      OT_CAST,

		OT_UNARY_ADD,
      OT_UNARY_LOGNOT,
		OT_UNARY_SUB,

		OT_VALUE_FLOAT,
		OT_VALUE_INT,
		OT_VALUE_OCODE,
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

   static Pointer create_branch_and(std::istream *in);
   static Pointer create_branch_ior(std::istream *in);
   static Pointer create_branch_xor(std::istream *in);

   static Pointer create_cast(std::istream *in);

	static Pointer create_unary_add(std::istream * in);
   static Pointer create_unary_lognot(std::istream *in);
	static Pointer create_unary_sub(std::istream * in);

	static Pointer create_value_float(std::istream * in);
	static Pointer create_value_int(std::istream * in);
	static Pointer create_value_ocode(std::istream * in);
	static Pointer create_value_symbol(std::istream * in);

private:
   virtual void writeACSPLong(std::ostream *out) const;


	static bigsint _address_count;

	static std::string _filename;

	static std::string _library_original;
	static std::set<std::string> _library_table;

	static std::map<std::string, ObjectData_Auto> _auto_table;

	static std::map<std::string, ObjectData_Function> _function_table;

	static std::map<std::string, ObjectData_Register> _register_global_table;
	static std::map<std::string, ObjectData_Register> _register_map_table;
	static std::map<std::string, ObjectData_Register> _register_world_table;

	static std::map<std::string, ObjectData_RegisterArray> _registerarray_global_table;
	static std::map<std::string, ObjectData_RegisterArray> _registerarray_map_table;
	static std::map<std::string, ObjectData_RegisterArray> _registerarray_world_table;

	static std::map<std::string, ObjectData_Script> _script_table;

	static std::map<std::string, ObjectData_Static> _static_table;

	static std::map<std::string, ObjectExpression::Pointer> _symbol_table;
	static std::map<std::string, ExpressionType>            _symbol_type_table;
};


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern bool option_string_fold;

#endif /* HPP_ObjectExpression_ */

