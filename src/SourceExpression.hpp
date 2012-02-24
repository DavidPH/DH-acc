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
// Base class for source-level expression handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_SourceExpression_
#define HPP_SourceExpression_

#include "bignum.hpp"
#include "Counter.hpp"
#include "SourcePosition.hpp"
#include "SourceVariable.hpp"

#include <ostream>
#include <string>
#include <vector>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define SRCEXP_EXPR_ARGS \
   SourceContext *context, SourcePosition const &position

#define SRCEXP_EXPR_PARM \
   SourceContext *_context, SourcePosition const &_position

#define SRCEXP_EXPR_PASS \
   _context, _position


#define SRCEXP_EXPRUNA_ARGS \
   SourceExpression *expr,  \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRUNA_PARM \
   SourceExpression *_expr, \
   SRCEXP_EXPR_PARM

#define SRCEXP_EXPRUNA_PASS \
   _expr,                   \
   SRCEXP_EXPR_PASS

#define SRCEXP_EXPRUNA_DECL(NAME)   \
   static SourceExpression::Pointer \
   create_unary_##NAME(SRCEXP_EXPRUNA_ARGS)

#define SRCEXP_EXPRUNA_DEFN(NAME)               \
   SourceExpression::Pointer SourceExpression:: \
   create_unary_##NAME(SRCEXP_EXPRUNA_ARGS)


#define SRCEXP_EXPRBIN_ARGS                          \
   SourceExpression *exprL, SourceExpression *exprR, \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRBIN_PARM                            \
   SourceExpression *_exprL, SourceExpression *_exprR, \
   SRCEXP_EXPR_PARM

#define SRCEXP_EXPRBIN_PASS \
   _exprL, _exprR,          \
   SRCEXP_EXPR_PASS

#define SRCEXP_EXPRBIN_DECL(NAME)   \
   static SourceExpression::Pointer \
   create_binary_##NAME(SRCEXP_EXPRBIN_ARGS)

#define SRCEXP_EXPRBIN_DEFN(NAME)               \
   SourceExpression::Pointer SourceExpression:: \
   create_binary_##NAME(SRCEXP_EXPRBIN_ARGS)


#define SRCEXP_EXPRBRA0_ARGS \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRBRA1_ARGS   \
   SourceExpression *exprCond, \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRBRA2_ARGS                               \
   SourceExpression *exprCond, SourceExpression *exprBody, \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRBRA3_ARGS                               \
   SourceExpression *exprCond, SourceExpression *exprBody, \
   SourceExpression *exprElse,                             \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRBRA4_ARGS                               \
   SourceExpression *exprCond, SourceExpression *exprBody, \
   SourceExpression *exprIter, SourceExpression *exprInit, \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRBRAu_ARGS \
   SourceExpression *expr,   \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRBRAb_ARGS                         \
   SourceExpression *exprL, SourceExpression *exprR, \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRBRAa_ARGS                                     \
   SourceExpression *expr, SourceExpression::Vector const &args, \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRBRAs_ARGS \
   std::string const &value, \
   SRCEXP_EXPR_ARGS

//
// SRCEXP_EXPRBRA_DECL
//
// Declares a create_branch function.
//
// Type is one of the following:
//   0 - No expressions.
//   1 - Condition expression.
//   2 - Condition, body expressions.
//   3 - Condition, body, else expressions.
//   4 - Condition, body, iterate, initialize expressions.
//   u - Single expression.
//   b - Left, right expressions.
//   a - Single expression and vector.
//   s - String argument.
//
#define SRCEXP_EXPRBRA_DECL(TYPE,NAME) \
   static SourceExpression::Pointer    \
   create_branch_##NAME(SRCEXP_EXPRBRA##TYPE##_ARGS)

//
// SRCEXP_EXPRBRA_DEFN
//
// Begins defining a create_branch function.
//
// Type is same as DECL.
//
#define SRCEXP_EXPRBRA_DEFN(TYPE,NAME)          \
   SourceExpression::Pointer SourceExpression:: \
   create_branch_##NAME(SRCEXP_EXPRBRA##TYPE##_ARGS)


#define SRCEXP_EXPRVALa_ARGS             \
   SourceExpression::Vector const &args, \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRVALc_ARGS \
   char value,               \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRVALe_ARGS \
   SourceExpression *expr,   \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRVALes_ARGS                        \
   SourceExpression *expr, std::string const &value, \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRVALet_ARGS                  \
   SourceExpression *expr, VariableType *type, \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRVALi_ARGS \
   bigsint value,            \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRVALr_ARGS \
   bigreal value,            \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRVALs_ARGS \
   std::string const &value, \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRVALt_ARGS \
   VariableType *type,       \
   SRCEXP_EXPR_ARGS

#define SRCEXP_EXPRVALv_ARGS \
   SourceVariable *var,      \
   SRCEXP_EXPR_ARGS

//
// SRCEXP_EXPRVAL_DECL
//
// Declares a create_value function.
//
// Type is one of the following:
//    a - Vector argument.
//    c - Char argument.
//   e  - Expression argument.
//   es - Expression, string arguments.
//   et - Expression, type arguments.
//    i - Integer argument.
//    r - Real argument.
//    s - String argument.
//    t - Type argument.
//    v - Variable argument.
//
#define SRCEXP_EXPRVAL_DECL(TYPE,NAME) \
   static SourceExpression::Pointer    \
   create_value_##NAME(SRCEXP_EXPRVAL##TYPE##_ARGS)

//
// SRCEXP_EXPRVAL_DEFN
//
// Begins defining a create_value function.
//
// Type is same as DECL.
//
#define SRCEXP_EXPRVAL_DEFN(TYPE,NAME)          \
   SourceExpression::Pointer SourceExpression:: \
   create_value_##NAME(SRCEXP_EXPRVAL##TYPE##_ARGS)


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ObjectExpression;
class ObjectVector;
class SourceContext;
class VariableData;
class VariableType;

//
// SourceExpression
//
class SourceExpression : public Counter
{
   MAKE_ABSTRACT_COUNTER_CLASS_BASE(SourceExpression, Counter);

public:
   typedef std::vector<Pointer> Vector;


   void addLabel(std::string const &label);

   virtual bool canGetData() const;

   virtual bool canMakeObject() const;

   virtual CounterPointer<VariableData> getData() const;

   virtual CounterReference<VariableType> getType() const;

   virtual CounterPointer<ObjectExpression> makeObject() const;

   void makeObjects(ObjectVector *objects, VariableData *dst);

   void makeObjectsCast
   (ObjectVector *objects, VariableData *dst, VariableType *dstType);


   friend class SourceVariable;

   // Unary operators.
   SRCEXP_EXPRUNA_DECL(dec_pre);
   SRCEXP_EXPRUNA_DECL(dec_suf);
   SRCEXP_EXPRUNA_DECL(dereference);
   SRCEXP_EXPRUNA_DECL(inc_pre);
   SRCEXP_EXPRUNA_DECL(inc_suf);
   SRCEXP_EXPRUNA_DECL(not);
   SRCEXP_EXPRUNA_DECL(reference);
   SRCEXP_EXPRUNA_DECL(sub);

   // Binary operators.
   SRCEXP_EXPRBIN_DECL(add);
   SRCEXP_EXPRBIN_DECL(add_eq);
   SRCEXP_EXPRBIN_DECL(and);
   SRCEXP_EXPRBIN_DECL(and_eq);
   SRCEXP_EXPRBIN_DECL(array);
   SRCEXP_EXPRBIN_DECL(assign);
   SRCEXP_EXPRBIN_DECL(assign_const);
   SRCEXP_EXPRBIN_DECL(div);
   SRCEXP_EXPRBIN_DECL(div_eq);
   SRCEXP_EXPRBIN_DECL(eq);
   SRCEXP_EXPRBIN_DECL(ge);
   SRCEXP_EXPRBIN_DECL(gt);
   SRCEXP_EXPRBIN_DECL(ior);
   SRCEXP_EXPRBIN_DECL(ior_eq);
   SRCEXP_EXPRBIN_DECL(le);
   SRCEXP_EXPRBIN_DECL(lsh);
   SRCEXP_EXPRBIN_DECL(lsh_eq);
   SRCEXP_EXPRBIN_DECL(lt);
   SRCEXP_EXPRBIN_DECL(mod);
   SRCEXP_EXPRBIN_DECL(mod_eq);
   SRCEXP_EXPRBIN_DECL(mul);
   SRCEXP_EXPRBIN_DECL(mul_eq);
   SRCEXP_EXPRBIN_DECL(ne);
   SRCEXP_EXPRBIN_DECL(rsh);
   SRCEXP_EXPRBIN_DECL(rsh_eq);
   SRCEXP_EXPRBIN_DECL(sub);
   SRCEXP_EXPRBIN_DECL(sub_eq);
   SRCEXP_EXPRBIN_DECL(xor);
   SRCEXP_EXPRBIN_DECL(xor_eq);

   // Branching operators.
   SRCEXP_EXPRBRA_DECL(b, and);
   SRCEXP_EXPRBRA_DECL(b, and_eq);
   SRCEXP_EXPRBRA_DECL(0, break);
   SRCEXP_EXPRBRA_DECL(a, call);
   SRCEXP_EXPRBRA_DECL(0, continue);
   SRCEXP_EXPRBRA_DECL(2, do);
   SRCEXP_EXPRBRA_DECL(4, for);
   SRCEXP_EXPRBRA_DECL(u, goto);
   SRCEXP_EXPRBRA_DECL(s, goto);
   SRCEXP_EXPRBRA_DECL(2, if);
   SRCEXP_EXPRBRA_DECL(3, if);
   SRCEXP_EXPRBRA_DECL(b, ior);
   SRCEXP_EXPRBRA_DECL(b, ior_eq);
   SRCEXP_EXPRBRA_DECL(u, not);
   SRCEXP_EXPRBRA_DECL(u, return);
   SRCEXP_EXPRBRA_DECL(2, switch);
   SRCEXP_EXPRBRA_DECL(2, while);
   SRCEXP_EXPRBRA_DECL(b, xor);
   SRCEXP_EXPRBRA_DECL(b, xor_eq);

   // Values.
   SRCEXP_EXPRVAL_DECL( a, block);
   SRCEXP_EXPRVAL_DECL(et, cast_explicit);
   SRCEXP_EXPRVAL_DECL(et, cast_implicit);
   SRCEXP_EXPRVAL_DECL( c, char);
   SRCEXP_EXPRVAL_DECL( s, char);
   SRCEXP_EXPRVAL_DECL( t, data);
   SRCEXP_EXPRVAL_DECL( t, data_garbage);
   SRCEXP_EXPRVAL_DECL( i, int);
   SRCEXP_EXPRVAL_DECL( s, int);
   SRCEXP_EXPRVAL_DECL(es, member);
   SRCEXP_EXPRVAL_DECL( r, real);
   SRCEXP_EXPRVAL_DECL( s, real);
   SRCEXP_EXPRVAL_DECL( s, string);
   SRCEXP_EXPRVAL_DECL( i, uint);
   SRCEXP_EXPRVAL_DECL( v, variable);

   // Root expressions.
   static Pointer create_root_delay(SourceExpression *expr, SRCEXP_EXPR_ARGS);
   static Pointer create_root_output(SourceExpression *expr, SRCEXP_EXPR_ARGS);
   static Pointer create_root_printf
   (std::string const &printfType, std::string const &format,
    Vector const &expressions, SRCEXP_EXPR_ARGS);
   static Pointer create_root_script (VariableType *type, SRCEXP_EXPR_ARGS);

   static CounterReference<VariableType> get_promoted_type
   (VariableType *type1, VariableType *type2, SourcePosition const &position);

protected:
   SourceExpression(SRCEXP_EXPR_ARGS);
   ~SourceExpression();

   void recurse_makeObjects(ObjectVector *objects, VariableData *dst);

   void recurse_makeObjectsCast
   (ObjectVector *objects, VariableData *dst, VariableType *dstType);

   SourcePosition position;
   std::string label;
   CounterReference<SourceContext> context;



   static void
   make_objects_call(ObjectVector *objects, VariableData *dst,
                     SourceExpression *expr,
                     std::vector<SourceExpression::Pointer> const &args,
                     ObjectExpression *stack, std::string const &labelReturn,
                     SourcePosition const &position);

   static CounterPointer<ObjectExpression> make_object_cast
   (ObjectExpression *src, VariableType *dstType, VariableType *srcType,
    SourcePosition const &position);

   static void make_objects_memcpy_cast
   (ObjectVector *objects, VariableData *dst, VariableData *src,
    VariableType *dstType, VariableType *srcType,
    SourcePosition const &position);

   static void
   make_objects_memcpy_post(ObjectVector *objects, VariableData *dst,
                            VariableData *src, SourcePosition const &position);
   static void
   make_objects_memcpy_post(ObjectVector *objects, VariableData *dup,
                            VariableData *dst, VariableData *src,
                            SourcePosition const &position);

   static void
   make_objects_memcpy_prep(ObjectVector *objects, VariableData *dst,
                            VariableData *src, SourcePosition const &position);
   static void
   make_objects_memcpy_prep(ObjectVector *objects, VariableData *dup,
                            VariableData *dst, VariableData *src,
                            SourcePosition const &position);

   static void
   make_objects_memcpy_void(ObjectVector *objects, VariableData *src,
                            SourcePosition const &position);

private:
   void makeObjectsBase(ObjectVector *objects, VariableData *dst);

   void recurse_makeObjectsBase(ObjectVector *objects, VariableData *dst);

   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   virtual void virtual_makeObjectsCast
   (ObjectVector *objects, VariableData *dst, VariableType *dstType);

   std::vector<std::string> labels;
   bool evaluated;



   static void make_objects_call_asmfunc
   (ObjectVector *objects, VariableData *dst, VariableType *type,
    ObjectExpression *data, Vector const &args, SourcePosition const &position);

   static void make_objects_call_function
   (ObjectVector *objects, VariableData *dst, VariableType *type,
    ObjectExpression *data, Vector const &args, ObjectExpression *stack,
    std::string const &labelReturn, SourcePosition const &position);

   static void  make_objects_call_function
   (ObjectVector *objects, VariableData *dst, VariableType *type,
    SourceExpression *data, Vector const &args, ObjectExpression *stack,
    std::string const &labelReturn, SourcePosition const &position);

   static void make_objects_call_linespec
   (ObjectVector *objects, VariableData *dst, VariableType *type,
    ObjectExpression *data, Vector const &args, SourcePosition const &position);

   static void make_objects_call_native
   (ObjectVector *objects, VariableData *dst, VariableType *type,
    ObjectExpression *data, Vector const &args, SourcePosition const &position);

   static void make_objects_call_script
   (ObjectVector *objects, VariableData *dst, VariableType *type,
    SourceExpression *data, Vector const &args, ObjectExpression *stack,
    SourcePosition const &position);
};


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern bool option_string_tag;

#endif//HPP_SourceExpression_

