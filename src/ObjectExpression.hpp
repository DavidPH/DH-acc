//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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

class ObjectArchive;
struct ObjectCodeSet;
class ObjectVector;
class SourceTokenC;

//
// ObjectExpression
//
class ObjectExpression : public Counter
{
   CounterPreambleAbstract(ObjectExpression, Counter);

public:
   typedef std::vector<std::string> VecStr;
   typedef std::vector<Pointer> Vector;

   enum ExpressionType
   {
      ET_FIX_HH,
      ET_FIX_H,
      ET_FIX, // bigreal
      ET_FIX_L,
      ET_FIX_LL,

      ET_FLT_HH,
      ET_FLT_H,
      ET_FLT, // bigreal
      ET_FLT_L,
      ET_FLT_LL,

      ET_INT_HH,
      ET_INT_H,
      ET_INT, // bigsint
      ET_INT_L,
      ET_INT_LL,

      ET_UNS_HH,
      ET_UNS_H,
      ET_UNS, // biguint
      ET_UNS_L,
      ET_UNS_LL,

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

   virtual biguint     resolveBinary(biguint part) const;
   virtual std::string resolveString() const;
   virtual std::string resolveSymbol() const;

   void writeACSP(std::ostream *out) const;


   friend std::string const &make_string(ExpressionType et);

   friend void OA_Override(ExpressionType &out, ExpressionType const &in);
   friend void OA_Override(Pointer        &out, Pointer        const &in);
   friend void OA_Override(Reference      &out, Reference      const &in);

   friend ObjectArchive &operator << (ObjectArchive &arc, ExpressionType &data);
   friend ObjectArchive &operator << (ObjectArchive &arc, Pointer        &data);
   friend ObjectArchive &operator << (ObjectArchive &arc, Reference      &data);

	static void add_address_count(bigsint const addressCount);

	// Adds a label for the current address count.
	static void add_label(std::string const & symbol);

	static void add_symbol(std::string const & symbol, ObjectExpression * value);
	static void add_symbol(std::string const & symbol, ExpressionType type);

   static ObjectArchive &Archive(ObjectArchive &arc, ObjectVector &objects);

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

   static Reference CreateValueFIX_HH(bigreal value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueFIX_H (bigreal value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueFIX   (bigreal value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueFIX_L (bigreal value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueFIX_LL(bigreal value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueFLT_HH(bigreal value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueFLT_H (bigreal value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueFLT   (bigreal value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueFLT_L (bigreal value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueFLT_LL(bigreal value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueINT_HH(bigsint value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueINT_H (bigsint value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueINT   (bigsint value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueINT_L (bigsint value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueINT_LL(bigsint value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueUNS_HH(biguint value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueUNS_H (biguint value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueUNS   (biguint value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueUNS_L (biguint value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueUNS_LL(biguint value, OBJEXP_EXPR_ARGS);
   static Reference CreateValueOCS(ObjectCodeSet const &ocode, OBJEXP_EXPR_ARGS);
   static Reference CreateValueARR(Vector const &elems, OBJEXP_EXPR_ARGS);
   static Reference CreateValueMAP(Vector const &elems, VecStr const &names, OBJEXP_EXPR_ARGS);
   static Reference CreateValueCast(ExpressionType type, OBJEXP_EXPRUNA_ARGS);
   static Reference CreateValuePart(biguint part, OBJEXP_EXPRUNA_ARGS);
   static Reference CreateValueSymbol(std::string const &symbol, OBJEXP_EXPR_ARGS);

	static void do_deferred_allocation();

	static bigsint get_address_count();

   static std::string const &get_filename();
   static std::string const &get_filename_raw();

	static ObjectExpression::Pointer get_symbol(std::string const & symbol, SourcePosition const & position);
   static ObjectExpression::Pointer get_symbol_null(std::string const &symbol);
	static ExpressionType get_symbol_type(std::string const & symbol, SourcePosition const & position);

	static void iter_library(void (*iter)(std::ostream *, std::string const &), std::ostream * out);

	static void set_address_count(bigsint addressCount);

	// Sets the current filename, translating to make it a valid symbol.
	static void set_filename(std::string const & filename);

	static void set_library(std::string const & library);

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

      OT_VALUE_FIX,
      OT_VALUE_FLT,
      OT_VALUE_INT,
      OT_VALUE_UNS,
      OT_VALUE_OCS,
      OT_VALUE_ARR,
      OT_VALUE_CAST,
      OT_VALUE_PART,
      OT_VALUE_SYMBOL,

      OT_NONE
   };

   explicit ObjectExpression(OBJEXP_EXPR_ARGS);
   explicit ObjectExpression(ObjectArchive &arc);

   virtual ObjectArchive &archive(ObjectArchive &arc);

   SourcePosition pos;


   friend ObjectArchive &operator << (ObjectArchive &arc, ObjectType &data);
   friend ObjectArchive &operator << (ObjectArchive &arc, ObjectType const &data);

   static Reference Create(ObjectArchive &arc);

   static Reference CreateUnaryAdd(ObjectArchive &arc);
   static Reference CreateUnaryNot(ObjectArchive &arc);
   static Reference CreateUnarySub(ObjectArchive &arc);

   static Reference CreateBinaryAdd(ObjectArchive &arc);
   static Reference CreateBinaryAnd(ObjectArchive &arc);
   static Reference CreateBinaryCmp(ObjectArchive &arc);
   static Reference CreateBinaryDiv(ObjectArchive &arc);
   static Reference CreateBinaryIOr(ObjectArchive &arc);
   static Reference CreateBinaryLSh(ObjectArchive &arc);
   static Reference CreateBinaryMod(ObjectArchive &arc);
   static Reference CreateBinaryMul(ObjectArchive &arc);
   static Reference CreateBinaryRSh(ObjectArchive &arc);
   static Reference CreateBinarySub(ObjectArchive &arc);
   static Reference CreateBinaryXOr(ObjectArchive &arc);

   static Reference CreateBranchAnd(ObjectArchive &arc);
   static Reference CreateBranchIf (ObjectArchive &arc);
   static Reference CreateBranchIOr(ObjectArchive &arc);
   static Reference CreateBranchNot(ObjectArchive &arc);
   static Reference CreateBranchXOr(ObjectArchive &arc);


   static Reference CreateValueFIX   (ObjectArchive &arc);
   static Reference CreateValueFLT   (ObjectArchive &arc);
   static Reference CreateValueINT   (ObjectArchive &arc);
   static Reference CreateValueUNS   (ObjectArchive &arc);
   static Reference CreateValueOCS   (ObjectArchive &arc);
   static Reference CreateValueARR   (ObjectArchive &arc);
   static Reference CreateValueCast  (ObjectArchive &arc);
   static Reference CreateValuePart  (ObjectArchive &arc);
   static Reference CreateValueSymbol(ObjectArchive &arc);

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

