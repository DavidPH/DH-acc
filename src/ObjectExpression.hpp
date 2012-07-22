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


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define OBJEXP_EXPR_ARGS SourcePosition const &pos
#define OBJEXP_EXPR_PARM SourcePosition const &_pos
#define OBJEXP_EXPR_PASS _pos

#define OBJEXP_EXPRUNA_ARGS ObjectExpression *expr, OBJEXP_EXPR_ARGS
#define OBJEXP_EXPRUNA_PARM ObjectExpression *_expr, OBJEXP_EXPR_PARM
#define OBJEXP_EXPRUNA_PASS _expr, OBJEXP_EXPR_PASS

#define OBJEXP_EXPRBIN_ARGS ObjectExpression *exprL, ObjectExpression *exprR, \
                            OBJEXP_EXPR_ARGS
#define OBJEXP_EXPRBIN_PARM ObjectExpression *_exprL, ObjectExpression *_exprR, \
                            OBJEXP_EXPR_PARM
#define OBJEXP_EXPRBIN_PASS _exprL, _exprR, OBJEXP_EXPR_PASS

#define OBJEXP_EXPRTRI_ARGS ObjectExpression *exprC, OBJEXP_EXPRBIN_ARGS
#define OBJEXP_EXPRTRI_PARM ObjectExpression *_exprC, OBJEXP_EXPRBIN_PARM
#define OBJEXP_EXPRTRI_PASS _exprC, OBJEXP_EXPRBIN_PASS


//----------------------------------------------------------------------------|
// Types                                                                      |
//

struct ObjectCodeSet;
class ObjectVector;
class SourceTokenC;

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
      ET_FIX, // bigreal
      ET_FLT, // bigreal
      ET_INT, // bigsint
      ET_UNS, // biguint
      ET_OCS, // ObjectCodeSet

      ET_ARR,
      ET_MAP,
   };


   virtual bool canResolve() const = 0;

   virtual void expand(Vector *out) {out->push_back(this);}
   virtual void expandOnce(Vector *out) {out->push_back(this);}

   SourcePosition const &getPosition() const {return pos;}

   virtual ExpressionType getType() const = 0;

   virtual bigreal resolveFIX() const;
   virtual bigreal resolveFLT() const;
   virtual bigsint resolveINT() const;
   virtual biguint resolveUNS() const;
   virtual ObjectCodeSet resolveOCS() const;

   virtual Reference resolveARR(biguint index) const;
   virtual Reference resolveMAP(std::string const &name) const;

   virtual biguint     resolveBinary() const;
   virtual std::string resolveString() const;
   virtual std::string resolveSymbol() const;

   void writeACSP(std::ostream *out) const;


   friend std::string const &make_string(ExpressionType et);

   friend void override_object(ExpressionType *out, ExpressionType const *in);
   friend void override_object(Pointer        *out, Pointer        const *in);
   friend void override_object(Reference      *out, Reference      const *in);

   friend void read_object(std::istream *in, ExpressionType *out);
   friend void read_object(std::istream *in, Pointer        *out);
   friend void read_object(std::istream *in, Reference      *out);

   friend void write_object(std::ostream *out, ExpressionType const *in);
   friend void write_object(std::ostream *out, Pointer        const *in);
   friend void write_object(std::ostream *out, Reference      const *in);

	static void add_address_count(bigsint const addressCount);

	// Adds a label for the current address count.
	static void add_label(std::string const & symbol);

	static void add_symbol(std::string const & symbol, ObjectExpression * value);
	static void add_symbol(std::string const & symbol, ExpressionType type);

   static Reference create_unary_add(OBJEXP_EXPRUNA_ARGS);
   static Reference create_unary_not(OBJEXP_EXPRUNA_ARGS);
   static Reference create_unary_sub(OBJEXP_EXPRUNA_ARGS);

   static Reference create_binary_add   (OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_and   (OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_cmp_ge(OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_cmp_gt(OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_cmp_le(OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_cmp_lt(OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_cmp_eq(OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_cmp_ne(OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_div   (OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_ior   (OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_lsh   (OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_mod   (OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_mul   (OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_rsh   (OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_sub   (OBJEXP_EXPRBIN_ARGS);
   static Reference create_binary_xor   (OBJEXP_EXPRBIN_ARGS);

   static Reference create_branch_and(OBJEXP_EXPRBIN_ARGS);
   static Reference create_branch_if (OBJEXP_EXPRTRI_ARGS);
   static Reference create_branch_ior(OBJEXP_EXPRBIN_ARGS);
   static Reference create_branch_not(OBJEXP_EXPRUNA_ARGS);
   static Reference create_branch_xor(OBJEXP_EXPRBIN_ARGS);

   static Reference create_cast_fix_to_flt(OBJEXP_EXPRUNA_ARGS);
   static Reference create_cast_fix_to_int(OBJEXP_EXPRUNA_ARGS);
   static Reference create_cast_fix_to_uns(OBJEXP_EXPRUNA_ARGS);
   static Reference create_cast_flt_to_fix(OBJEXP_EXPRUNA_ARGS);
   static Reference create_cast_flt_to_int(OBJEXP_EXPRUNA_ARGS);
   static Reference create_cast_flt_to_uns(OBJEXP_EXPRUNA_ARGS);
   static Reference create_cast_int_to_fix(OBJEXP_EXPRUNA_ARGS);
   static Reference create_cast_int_to_flt(OBJEXP_EXPRUNA_ARGS);
   static Reference create_cast_int_to_uns(OBJEXP_EXPRUNA_ARGS);
   static Reference create_cast_uns_to_fix(OBJEXP_EXPRUNA_ARGS);
   static Reference create_cast_uns_to_flt(OBJEXP_EXPRUNA_ARGS);
   static Reference create_cast_uns_to_int(OBJEXP_EXPRUNA_ARGS);

   static Reference create_value_fix(bigreal value, SourcePosition const &pos);
   static Reference create_value_flt(bigreal value, SourcePosition const &pos);
   static Reference create_value_int(bigsint value, SourcePosition const &pos);
   static Reference create_value_uns(biguint value, SourcePosition const &pos);
   static Reference create_value_ocs(ObjectCodeSet const &ocode, SourcePosition const &pos);
   static Reference create_value_arr(Vector const &elems, SourcePosition const &pos);
   static Reference create_value_map(Vector const &elems, VecStr const &names, SourcePosition const &pos);
   static Reference create_value_symbol(std::string const &symbol, SourcePosition const &pos);

	static void do_deferred_allocation();

	static bigsint get_address_count();

   static std::string const &get_filename();
   static std::string const &get_filename_raw();

	static ObjectExpression::Pointer get_symbol(std::string const & symbol, SourcePosition const & position);
   static ObjectExpression::Pointer get_symbol_null(std::string const &symbol);
	static ExpressionType get_symbol_type(std::string const & symbol, SourcePosition const & position);

	static void iter_library(void (*iter)(std::ostream *, std::string const &), std::ostream * out);

	static void read_objects(std::istream * in, ObjectVector * objects);

	static void set_address_count(bigsint addressCount);

	// Sets the current filename, translating to make it a valid symbol.
	static void set_filename(std::string const & filename);

	static void set_library(std::string const & library);

   static void write_objects(std::ostream *out, ObjectVector const *objects);

protected:
   enum ObjectType
   {
      OT_UNARY_ADD,
      OT_UNARY_NOT,
      OT_UNARY_SUB,

      OT_BINARY_ADD,
      OT_BINARY_AND,
      OT_BINARY_CMP,
      OT_BINARY_DIV,
      OT_BINARY_IOR,
      OT_BINARY_LSH,
      OT_BINARY_MOD,
      OT_BINARY_MUL,
      OT_BINARY_RSH,
      OT_BINARY_SUB,
      OT_BINARY_XOR,

      OT_BRANCH_AND,
      OT_BRANCH_IF,
      OT_BRANCH_IOR,
      OT_BRANCH_NOT,
      OT_BRANCH_XOR,

      OT_CAST,

      OT_VALUE_FIX,
      OT_VALUE_FLT,
      OT_VALUE_INT,
      OT_VALUE_UNS,
      OT_VALUE_OCS,
      OT_VALUE_ARR,
      OT_VALUE_SYMBOL,

      OT_NONE
   };

   ObjectExpression(SourcePosition const &pos);
   ObjectExpression(std::istream *in);

   virtual void writeObject(std::ostream *out) const;

   SourcePosition pos;



   friend void read_object(std::istream *in, ObjectType *out);

   friend void write_object(std::ostream *out, ObjectType const *in);
   friend void write_object(std::ostream *out, ObjectType const &in);

   static Reference create(std::istream *in);

   static Reference create_unary_add(std::istream *in);
   static Reference create_unary_not(std::istream *in);
   static Reference create_unary_sub(std::istream *in);

   static Reference create_binary_add(std::istream *in);
   static Reference create_binary_and(std::istream *in);
   static Reference create_binary_cmp(std::istream *in);
   static Reference create_binary_div(std::istream *in);
   static Reference create_binary_ior(std::istream *in);
   static Reference create_binary_lsh(std::istream *in);
   static Reference create_binary_mod(std::istream *in);
   static Reference create_binary_mul(std::istream *in);
   static Reference create_binary_rsh(std::istream *in);
   static Reference create_binary_sub(std::istream *in);
   static Reference create_binary_xor(std::istream *in);

   static Reference create_branch_and(std::istream *in);
   static Reference create_branch_if (std::istream *in);
   static Reference create_branch_ior(std::istream *in);
   static Reference create_branch_not(std::istream *in);
   static Reference create_branch_xor(std::istream *in);

   static Reference create_cast(std::istream *in);

   static Reference create_value_fix(std::istream *in);
   static Reference create_value_flt(std::istream *in);
   static Reference create_value_int(std::istream *in);
   static Reference create_value_uns(std::istream *in);
   static Reference create_value_ocs(std::istream *in);
   static Reference create_value_arr(std::istream *in);
   static Reference create_value_symbol(std::istream *in);

private:
   virtual void writeACSPLong(std::ostream *out) const;


   static bigsint address_count;

   static std::string filename, filename_raw;

   static std::set<std::string> library_table;

   static std::map<std::string, Pointer>        symbol_table;
   static std::map<std::string, ExpressionType> symbol_type_table;
};


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern int option_addr_array;
extern int option_addr_stack;

extern bool option_string_fold;
extern bool option_string_tag;

#endif//HPP_ObjectExpression_

