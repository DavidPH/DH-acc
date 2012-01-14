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

class ObjectExpression;
class ObjectVector;
class SourceContext;
class SourceTokenC;
class VariableData;
struct VariableType;


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression
//
class SourceExpression : public Counter
{
   MAKE_ABSTRACT_COUNTER_CLASS_BASE(SourceExpression, Counter);

public:
   void addLabel(std::string const &label);

   virtual bool canGetData() const;

   virtual bool canMakeObject() const;
   virtual bool canMakeObjectAddress() const;

   virtual bool canMakeObjectsAddress() const;

   virtual CounterPointer<VariableData> getData() const;

   virtual VariableType const *getType() const;

   virtual CounterPointer<ObjectExpression> makeObject() const;
   virtual CounterPointer<ObjectExpression> makeObjectAddress() const;

   void makeObjects(ObjectVector *objects, VariableData *dst);

   void makeObjectsAddress(ObjectVector *objects, VariableData *dst);

   void makeObjectsCast(ObjectVector *objects, VariableData *dst,
                        VariableType const *dstType);


   friend class SourceVariable;

   static Pointer create_binary_add(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_and(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_array(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_assign(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_assign_const(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_div(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_eq(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_ge(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_gt(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_ior(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_le(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_lt(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_mod(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_mul(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_ne(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_shiftl(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_shiftr(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_sub(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);
   static Pointer create_binary_xor(SourceExpression *exprL, SourceExpression *exprR, SourcePosition const &position);

   static Pointer create_branch_and(SourceExpression *exprL, SourceExpression *exprR, SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_break(SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_call(SourceExpression *expr, std::vector<SourceExpression::Pointer> const &args, SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_continue(SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_do(SourceExpression *exprCondition, SourceExpression *exprLoop, SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_for(SourceExpression *exprInit, SourceExpression *exprCond, SourceExpression *exprIter, SourceExpression *exprLoop, SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_goto(std::string const &label, SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_goto(SourceExpression *expr, SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_if(SourceExpression *exprCondition, SourceExpression *exprIf, SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_if(SourceExpression *exprCondition, SourceExpression *exprIf, SourceExpression *exprElse, SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_ior(SourceExpression *exprL, SourceExpression *exprR, SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_not(SourceExpression *expr, SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_return(SourceExpression *expr, SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_switch(SourceExpression *expr, SourceExpression *exprCases, SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_while(SourceExpression *exprCondition, SourceExpression *exprWhile, SourceContext *context, SourcePosition const &position);
   static Pointer create_branch_xor(SourceExpression *exprL, SourceExpression *exprR, SourceContext *context, SourcePosition const &position);

   static Pointer create_unary_dec_pre(SourceExpression *expr, SourcePosition const &position);
   static Pointer create_unary_dec_suf(SourceExpression *expr, SourcePosition const &position);
   static Pointer create_unary_dereference(SourceExpression *expr, SourcePosition const &position);
   static Pointer create_unary_inc_pre(SourceExpression *expr, SourcePosition const &position);
   static Pointer create_unary_inc_suf(SourceExpression *expr, SourcePosition const &position);
   static Pointer create_unary_not(SourceExpression *expr, SourcePosition const &position);
   static Pointer create_unary_reference(SourceExpression *expr, SourcePosition const &position);
   static Pointer create_unary_sub(SourceExpression *expr, SourcePosition const &position);

   static Pointer create_root_delay(SourceExpression *expr, SourceContext *context, SourcePosition const &position);
   static Pointer create_root_output(SourceExpression *expr, SourcePosition const &position);
   static Pointer create_root_script(VariableType const *type, SourcePosition const &position);

   static Pointer create_value_block(std::vector<SourceExpression::Pointer> const &expressions, SourcePosition const &position);
   static Pointer create_value_cast(SourceExpression *expr, VariableType const *type, SourcePosition const &position);
   static Pointer create_value_char(SourceTokenC const &token);
   static Pointer create_value_data(VariableType const *type, bool garbage, SourcePosition const &position);
   static Pointer create_value_int(bigsint value, SourcePosition const &position);
   static Pointer create_value_int(SourceTokenC const &token);
   static Pointer create_value_member(SourceExpression *expr, SourceTokenC const &token);
   static Pointer create_value_real(SourceTokenC const &token);
   static Pointer create_value_string(SourceTokenC const &token);
   static Pointer create_value_variable(SourceVariable *var, SourcePosition const &position);

   static VariableType const *
   get_promoted_type(VariableType const *type1, VariableType const *type2,
                     SourcePosition const &position);

protected:
   SourceExpression(SourcePosition const & position);

   void
   recurse_makeObjects(ObjectVector *objects, VariableData *dst);

   void
   recurse_makeObjectsAddress(ObjectVector *objects, VariableData *dst);

   void
   recurse_makeObjectsCast(ObjectVector *objects, VariableData *dst,
                           VariableType const *dstType);

   SourcePosition position;



   static void
   make_objects_call(ObjectVector *objects, VariableData *dst,
                     SourceExpression *expr,
                     std::vector<SourceExpression::Pointer> const &args,
                     ObjectExpression *stack, std::string const &labelReturn,
                     SourcePosition const &position);

   static void
   make_objects_memcpy_cast(ObjectVector *objects,
                            VariableData *dst, VariableData *src,
                            VariableType const *dstType,
                            VariableType const *srcType,
                            SourcePosition const &position);

   static void
   make_objects_memcpy_post(ObjectVector *objects, VariableData *dst,
                            VariableData *src, SourcePosition const &position);

   static void
   make_objects_memcpy_prep(ObjectVector *objects, VariableData *dst,
                            VariableData *src, SourcePosition const &position);

   static void
   make_objects_memcpy_void(ObjectVector *objects, VariableData *src,
                            SourcePosition const &position);

private:
   SourceExpression();

   void makeObjectsBase(ObjectVector *objects, VariableData *dst);

   void recurse_makeObjectsBase(ObjectVector *objects, VariableData *dst);

   virtual void
   virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   virtual void
   virtual_makeObjectsAddress(ObjectVector *objects, VariableData *dst);

   virtual void
   virtual_makeObjectsCast(ObjectVector *objects, VariableData *dst,
                          VariableType const *dstType);

   std::vector<std::string> labels;
   bool evaluated;



   static void
   make_objects_call_asmfunc(ObjectVector *objects, VariableData *dst,
                             VariableType const *type, ObjectExpression *data,
                             std::vector<SourceExpression::Pointer> const &args,
                             SourcePosition const &position);

   static void
   make_objects_call_function(ObjectVector *objects, VariableData *dst,
                              VariableType const *type, ObjectExpression *data,
                              std::vector<SourceExpression::Pointer> const &args,
                              ObjectExpression *stack,
                              std::string const &labelReturn,
                              SourcePosition const &position);
   static void
   make_objects_call_function(ObjectVector *objects, VariableData *dst,
                              VariableType const *type, SourceExpression *data,
                              std::vector<SourceExpression::Pointer> const &args,
                              ObjectExpression *stack,
                              std::string const &labelReturn,
                              SourcePosition const &position);

   static void
   make_objects_call_linespec(ObjectVector *objects, VariableData *dst,
                              VariableType const *type, ObjectExpression *data,
                              std::vector<SourceExpression::Pointer> const &args,
                              SourcePosition const &position);

   static void
   make_objects_call_native(ObjectVector *objects, VariableData *dst,
                            VariableType const *type, ObjectExpression *data,
                            std::vector<SourceExpression::Pointer> const &args,
                            SourcePosition const &position);

   static void
   make_objects_call_script(ObjectVector *objects, VariableData *dst,
                            VariableType const *type, SourceExpression *data,
                            std::vector<SourceExpression::Pointer> const &args,
                            ObjectExpression *stack,
                            SourcePosition const &position);
};

#endif//HPP_SourceExpression_

