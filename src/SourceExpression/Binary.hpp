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
// Base class for source-level binary expression handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_Binary__SourceExpression_
#define HPP_Binary__SourceExpression_

#include "../SourceExpression.hpp"

#include "../ObjectCode.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// CONSTRUCTOR_TYPE_VARS
//
#define CONSTRUCTOR_TYPE_VARS()                      \
VariableType::Reference typeL = exprL->getType();    \
VariableType::Reference typeR = exprR->getType();    \
                                                     \
VariableType::BasicType btL = typeL->getBasicType(); \
VariableType::BasicType btR = typeR->getBasicType();

//
// CONSTRUCTOR_ARRAY_DECAY
//
#define CONSTRUCTOR_ARRAY_DECAY()                                  \
/* Array types decay to pointer types. */                          \
if (btL == VariableType::BT_ARR)                                   \
{                                                                  \
   btL = VariableType::BT_PTR;                                     \
   typeL = typeL->getReturn()->getPointer();                       \
   exprL = create_value_cast_implicit(exprL, typeL, context, pos); \
}                                                                  \
                                                                   \
if (btR == VariableType::BT_ARR)                                   \
{                                                                  \
   btR = VariableType::BT_PTR;                                     \
   typeR = typeR->getReturn()->getPointer();                       \
   exprR = create_value_cast_implicit(exprR, typeR, context, pos); \
}

//
// CONSTRUCTOR_POINTER_PREAMBLE
//
#define CONSTRUCTOR_POINTER_PREAMBLE()                    \
VariableType::Reference type = getType();                 \
                                                          \
if (type->getBasicType() != VariableType::BT_PTR) return; \
                                                          \
bigsint retnSize = type->getReturn()->getSize(pos);       \
                                                          \
if(retnSize == 1 && type->getSize(pos) == 1) return;      \
                                                          \
SourceExpression::Pointer exprSize =                      \
   create_value_uint(retnSize, context, pos);

//
// CONSTRAINT_ARITHMETIC
//
#define CONSTRAINT_ARITHMETIC(OPER)           \
if (!VariableType::is_bt_arithmetic(btL))     \
   Error_N(pos, "non-arithmetic " OPER " X"); \
                                              \
if (!VariableType::is_bt_arithmetic(btR))     \
   Error_N(pos, "X " OPER " non-arithmetic");

//
// CONSTRAINT_INTEGER
//
#define CONSTRAINT_INTEGER(OPER)           \
if (!VariableType::is_bt_integer(btL))     \
   Error_N(pos, "non-integer " OPER " X"); \
                                           \
if (!VariableType::is_bt_integer(btR))     \
   Error_N(pos, "X " OPER " non-integer");

//
// DO_GET_CASES
//
#define DO_GET_CASES(OP)                                                    \
case VariableType::BT_CHR: objects->addToken(OCODE_##OP##_STK_I); break;    \
                                                                            \
case VariableType::BT_FIX_HH: objects->addToken(OCODE_##OP##_STK_X); break; \
case VariableType::BT_FIX_H: objects->addToken(OCODE_##OP##_STK_X); break;  \
case VariableType::BT_FIX: objects->addToken(OCODE_##OP##_STK_X); break;    \
case VariableType::BT_FIX_L: objects->addToken(OCODE_##OP##_STK_X); break;  \
case VariableType::BT_FIX_LL: objects->addToken(OCODE_##OP##_STK_X); break; \
                                                                            \
case VariableType::BT_INT_HH: objects->addToken(OCODE_##OP##_STK_I); break; \
case VariableType::BT_INT_H: objects->addToken(OCODE_##OP##_STK_I); break;  \
case VariableType::BT_INT: objects->addToken(OCODE_##OP##_STK_I); break;    \
                                                                            \
case VariableType::BT_UNS_HH: objects->addToken(OCODE_##OP##_STK_U); break; \
case VariableType::BT_UNS_H: objects->addToken(OCODE_##OP##_STK_U); break;  \
case VariableType::BT_UNS: objects->addToken(OCODE_##OP##_STK_U); break;    \
                                                                            \
case VariableType::BT_PTR: objects->addToken(OCODE_##OP##_STK_U); break;    \
                                                                            \
default: Error_NP("invalid BT: %s", make_string(type->getBasicType()).c_str())

//
// DO_GET_SWITCH
//
#define DO_GET_SWITCH(OP) \
switch (type->getBasicType()) {DO_GET_CASES(OP);}

//
// DO_SET_CASES
//
#define DO_SET_CASES(OP,MT) \
case VariableType::BT_CHR: objects->addToken(OCODE_##OP##_##MT##_I, data->address); break; \
 \
case VariableType::BT_FIX_HH: objects->addToken(OCODE_##OP##_##MT##_X, data->address); break; \
case VariableType::BT_FIX_H: objects->addToken(OCODE_##OP##_##MT##_X, data->address); break; \
case VariableType::BT_FIX: objects->addToken(OCODE_##OP##_##MT##_X, data->address); break; \
case VariableType::BT_FIX_L: objects->addToken(OCODE_##OP##_##MT##_X, data->address); break; \
case VariableType::BT_FIX_LL: objects->addToken(OCODE_##OP##_##MT##_X, data->address); break; \
 \
case VariableType::BT_INT_HH: objects->addToken(OCODE_##OP##_##MT##_I, data->address); break; \
case VariableType::BT_INT_H: objects->addToken(OCODE_##OP##_##MT##_I, data->address); break; \
case VariableType::BT_INT: objects->addToken(OCODE_##OP##_##MT##_I, data->address); break; \
 \
case VariableType::BT_UNS_HH: objects->addToken(OCODE_##OP##_##MT##_U, data->address); break; \
case VariableType::BT_UNS_H: objects->addToken(OCODE_##OP##_##MT##_U, data->address); break; \
case VariableType::BT_UNS: objects->addToken(OCODE_##OP##_##MT##_U, data->address); break; \
 \
case VariableType::BT_PTR: objects->addToken(OCODE_##OP##_##MT##_U, data->address); break; \
 \
default: Error_NP("invalid BT: %s", make_string(type->getBasicType()).c_str())

//
// DO_SET_SWITCH
//
#define DO_SET_SWITCH(OP,MT) \
switch (type->getBasicType()) {DO_SET_CASES(OP, MT);}

//
// DO_SET_SWITCHES
//
#define DO_SET_SWITCHES(OP) \
switch (data->type) { \
case VariableData::MT_STATIC: DO_SET_SWITCH(OP, STATIC); break; \
case VariableData::MT_AUTO: DO_SET_SWITCH(OP, AUTO); break; \
case VariableData::MT_POINTER: DO_SET_SWITCH(OP, PTR); break; \
case VariableData::MT_REGISTER: \
   switch (data->sectionR) { \
   case VariableData::SR_LOCAL: DO_SET_SWITCH(OP, REG); break; \
   case VariableData::SR_MAP: DO_SET_SWITCH(OP, MAPREG); break; \
   case VariableData::SR_WORLD: DO_SET_SWITCH(OP, WLDREG); break; \
   case VariableData::SR_GLOBAL: DO_SET_SWITCH(OP, GBLREG); break; \
   } break; \
case VariableData::MT_ARRAY: \
   switch (data->sectionA) { \
   case VariableData::SA_MAP: DO_SET_SWITCH(OP, MAPARR); break; \
   case VariableData::SA_WORLD: DO_SET_SWITCH(OP, WLDARR); break; \
   case VariableData::SA_GLOBAL: DO_SET_SWITCH(OP, GBLARR); break; \
   } break; \
default: Error_NP("invalid MT"); }

//
// CAN_SET_CASES
//
#define CAN_SET_CASES(OP,MT) \
case VariableType::BT_CHR: return true; \
 \
case VariableType::BT_FIX_HH: return true; \
case VariableType::BT_FIX_H: return true; \
case VariableType::BT_FIX: return true; \
case VariableType::BT_FIX_L: return true; \
case VariableType::BT_FIX_LL: return true; \
 \
case VariableType::BT_INT_HH: return true; \
case VariableType::BT_INT_H: return true; \
case VariableType::BT_INT: return true; \
case VariableType::BT_INT_L: return false; \
case VariableType::BT_INT_LL: return false; \
 \
case VariableType::BT_UNS_HH: return true; \
case VariableType::BT_UNS_H: return true; \
case VariableType::BT_UNS: return true; \
case VariableType::BT_UNS_L: return false; \
case VariableType::BT_UNS_LL: return false; \
 \
case VariableType::BT_PTR: return true; \
 \
default: return false

//
// CAN_SET_SWITCH
//
#define CAN_SET_SWITCH(OP,MT) \
switch (type->getBasicType()) {CAN_SET_CASES(OP, MT);}

//
// CAN_SET_SWITCHES
//
#define CAN_SET_SWITCHES(OP) \
switch (data->type) { \
case VariableData::MT_STATIC: CAN_SET_SWITCH(OP, STATIC); \
case VariableData::MT_AUTO: CAN_SET_SWITCH(OP, AUTO); \
case VariableData::MT_POINTER: CAN_SET_SWITCH(OP, PTR); \
case VariableData::MT_REGISTER:  \
   switch (data->sectionR) { \
   case VariableData::SR_LOCAL: CAN_SET_SWITCH(OP, REG); \
   case VariableData::SR_MAP: CAN_SET_SWITCH(OP, MAPREG); \
   case VariableData::SR_WORLD: CAN_SET_SWITCH(OP, WLDREG); \
   case VariableData::SR_GLOBAL: CAN_SET_SWITCH(OP, GBLREG); \
   } return false; \
case VariableData::MT_ARRAY: \
   switch (data->sectionA) { \
   case VariableData::SA_MAP: CAN_SET_SWITCH(OP, MAPARR); \
   case VariableData::SA_WORLD: CAN_SET_SWITCH(OP, WLDARR); \
   case VariableData::SA_GLOBAL: CAN_SET_SWITCH(OP, GBLARR); \
   } return false; \
default: return false; }

//
// EVALUATE_OBJECTS
//
#define EVALUATE_OBJECTS()                                              \
VariableType::Reference type = getType();                               \
ObjectExpression::Pointer objL =                                        \
   create_value_cast_explicit(exprL, type, context, pos)->makeObject(); \
ObjectExpression::Pointer objR =                                        \
   create_value_cast_explicit(exprR, type, context, pos)->makeObject();


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_Binary
//
class SourceExpression_Binary : public SourceExpression
{
   MAKE_ABSTRACT_COUNTER_CLASS_BASE(SourceExpression_Binary, SourceExpression);

public:
   virtual bool canMakeObject() const;

   virtual CounterReference<VariableType> getType() const;

protected:
   SourceExpression_Binary(SRCEXP_EXPRBIN_ARGS);

   SourceExpression_Binary(SRCEXP_EXPRBIN_ARGS, VariableType *castL,
                           VariableType *castR, bool assign);

   virtual bool canDoSet(VariableData *data, VariableType *type) const;

   virtual void doGet(ObjectVector *objects, VariableType *type, int tmpBase);

   void doGetBaseILAS(ObjectVector *objects, VariableType *type, int tmpBase, bool add);

   void doGetBaseILB(ObjectVector *objects, VariableType *type, int tmpBase,
                      ObjectCode ocode);

   virtual void doSet(ObjectVector *objects, VariableData *data,
                      VariableType *type, int tmpBase);

   void swapExpr()
   {
      SourceExpression::Pointer
      exprT = exprL;
      exprL = exprR;
      exprR = exprT;
   }

   SourceExpression::Pointer exprL;
   SourceExpression::Pointer exprR;

   bool assign;
   bool docast : 1;

private:
   void doGetBase(ObjectVector *objects, VariableData *dst);
   void doSetBase(ObjectVector *objects, VariableData *dst);
   void doSetBaseEmulated(ObjectVector *objects, VariableData *dst,
                          VariableData *src, VariableType *typeL);
   void doSetBaseGet(ObjectVector *objects, VariableData *src,
                     ObjectExpression *tmpA, ObjectExpression *tmpB);
   void doSetBaseSet(ObjectVector *objects, VariableData *src,
                     ObjectExpression *tmpA, ObjectExpression *tmpB);

   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};

#endif//HPP_Binary__SourceExpression_

