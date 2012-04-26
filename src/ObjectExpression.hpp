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
#include "SourcePosition.hpp"

#include <istream>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>

struct ObjectCodeSet;
struct ObjectData_Auto;
struct ObjectData_Static;
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
   typedef std::vector<std::string> VecStr;
   typedef std::vector<Pointer> Vector;

   enum ExpressionType
   {
      ET_ARRAY,
      ET_OCODE,
      ET_FLOAT,
      ET_INT,
      ET_STRUCT
   };


   virtual bool canResolve() const = 0;

   virtual void expand(Vector *out) {out->push_back(this);}
   virtual void expandOnce(Vector *out) {out->push_back(this);}

   SourcePosition const &getPosition() const;

   virtual ExpressionType getType() const = 0;

   virtual ObjectExpression::Pointer resolveElement(bigsint index) const;
   virtual bigreal resolveFloat() const;
   virtual bigsint resolveInt() const;
   virtual ObjectExpression::Pointer resolveMember(std::string const &name) const;
   virtual ObjectCodeSet resolveOCode() const;
   virtual std::string resolveSymbol() const;

   void writeACSP(std::ostream *out) const;


   friend void override_object(ExpressionType *out, ExpressionType const *in);
   friend void override_object(Pointer        *out, Pointer        const *in);

	friend void read_object(std::istream * in, ExpressionType * out);
	friend void read_object(std::istream * in, Pointer * out);

   friend void write_object(std::ostream *out, ExpressionType const *in);
   friend void write_object(std::ostream *out, Pointer        const *in);

	static void add_address_count(bigsint const addressCount);

	static void add_auto(std::string const & name, bigsint size, bigsint number);

	// Adds a label for the current address count.
	static void add_label(std::string const & symbol);

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
   static Pointer create_branch_not(ObjectExpression *expr, SourcePosition const &position);
   static Pointer create_branch_xor(ObjectExpression *exprL, ObjectExpression *exprR, SourcePosition const &position);

   static Pointer create_cast_float_to_int(ObjectExpression *expr, SourcePosition const &position);
   static Pointer create_cast_int_to_float(ObjectExpression *expr, SourcePosition const &position);

	static Pointer create_unary_add(ObjectExpression * expr, SourcePosition const & position);
	static Pointer create_unary_sub(ObjectExpression * expr, SourcePosition const & position);

   static Pointer create_value_array(Vector const &elems, SourcePosition const &position);
   static Pointer create_value_ocode(ObjectCodeSet const &ocode, SourcePosition const &position);
   static Pointer create_value_float(bigreal value, SourcePosition const &position);
   static Pointer create_value_int(bigsint value, SourcePosition const &position);
   static Pointer create_value_struct(Vector const &elems, VecStr const &names, SourcePosition const &position);
   static Pointer create_value_symbol(std::string const &symbol, SourcePosition const &position);

	static void do_deferred_allocation();

	static bigsint get_address_count();

	static std::string const & get_filename();

	static ObjectExpression::Pointer get_symbol(std::string const & symbol, SourcePosition const & position);
   static ObjectExpression::Pointer get_symbol_null(std::string const &symbol);
	static ExpressionType get_symbol_type(std::string const & symbol, SourcePosition const & position);

	static void iter_auto(void (*iter)(std::ostream *, ObjectData_Auto const &), std::ostream * out);

	static void iter_library(void (*iter)(std::ostream *, std::string const &), std::ostream * out);

	static void iter_static(void (*iter)(std::ostream *, ObjectData_Static const &), std::ostream * out);

	static void read_objects(std::istream * in, ObjectVector * objects);

	static void set_address_count(bigsint addressCount);

	// Sets the current filename, translating to make it a valid symbol.
	static void set_filename(std::string const & filename);

	static void set_library(std::string const & library);

   static void write_objects(std::ostream *out, ObjectVector const *objects);

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
      OT_BRANCH_NOT,
      OT_BRANCH_XOR,

      OT_CAST,

      OT_UNARY_ADD,
      OT_UNARY_SUB,

      OT_VALUE_COMPOUND,
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

   friend void write_object(std::ostream *out, ObjectType const *in);
   friend void write_object(std::ostream *out, ObjectType const &in);

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
   static Pointer create_branch_not(std::istream *in);
   static Pointer create_branch_xor(std::istream *in);

   static Pointer create_cast(std::istream *in);

	static Pointer create_unary_add(std::istream * in);
	static Pointer create_unary_sub(std::istream * in);

   static Pointer create_value_compound(std::istream *in);
   static Pointer create_value_float(std::istream *in);
   static Pointer create_value_int(std::istream *in);
   static Pointer create_value_ocode(std::istream *in);
   static Pointer create_value_symbol(std::istream *in);

private:
   virtual void writeACSPLong(std::ostream *out) const;


	static bigsint _address_count;

	static std::string _filename;

	static std::set<std::string> _library_table;

	static std::map<std::string, ObjectData_Auto> _auto_table;

	static std::map<std::string, ObjectData_Static> _static_table;

	static std::map<std::string, ObjectExpression::Pointer> _symbol_table;
	static std::map<std::string, ExpressionType>            _symbol_type_table;
};


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern int option_addr_array;
extern int option_addr_stack;
extern int option_static_temp;

extern bool option_string_fold;

#endif /* HPP_ObjectExpression_ */

