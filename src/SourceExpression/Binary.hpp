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
if (retnSize == 1) return;                                \
                                                          \
SourceExpression::Pointer exprSize =                      \
   create_value_uint(retnSize, context, pos);

//
// CONSTRAINT_ARITHMETIC
//
#define CONSTRAINT_ARITHMETIC(OPER)           \
if (!VariableType::is_bt_arithmetic(btL))     \
   ERROR_N(pos, "non-arithmetic " OPER " X"); \
                                              \
if (!VariableType::is_bt_arithmetic(btR))     \
   ERROR_N(pos, "X " OPER " non-arithmetic");

//
// CONSTRAINT_INTEGER
//
#define CONSTRAINT_INTEGER(OPER)           \
if (!VariableType::is_bt_integer(btL))     \
   ERROR_N(pos, "non-integer " OPER " X"); \
                                           \
if (!VariableType::is_bt_integer(btR))     \
   ERROR_N(pos, "X " OPER " non-integer");

//
// DO_GET_CASES
//
#define DO_GET_CASES(OP,PRE,SUFF,SUFI,SUFU)                                    \
case VariableType::BT_CHR: objects->addToken(OCODE_##PRE##OP##SUFI); break;    \
                                                                               \
case VariableType::BT_FIX_HH: objects->addToken(OCODE_##PRE##OP##SUFF); break; \
case VariableType::BT_FIX_H: objects->addToken(OCODE_##PRE##OP##SUFF); break;  \
case VariableType::BT_FIX: objects->addToken(OCODE_##PRE##OP##SUFF); break;    \
case VariableType::BT_FIX_L: objects->addToken(OCODE_##PRE##OP##SUFF); break;  \
case VariableType::BT_FIX_LL: objects->addToken(OCODE_##PRE##OP##SUFF); break; \
                                                                               \
case VariableType::BT_INT_HH: objects->addToken(OCODE_##PRE##OP##SUFI); break; \
case VariableType::BT_INT_H: objects->addToken(OCODE_##PRE##OP##SUFI); break;  \
case VariableType::BT_INT: objects->addToken(OCODE_##PRE##OP##SUFI); break;    \
case VariableType::BT_INT_L: objects->addToken(OCODE_##PRE##OP##SUFI); break;  \
                                                                               \
case VariableType::BT_UNS_HH: objects->addToken(OCODE_##PRE##OP##SUFU); break; \
case VariableType::BT_UNS_H: objects->addToken(OCODE_##PRE##OP##SUFU); break;  \
case VariableType::BT_UNS: objects->addToken(OCODE_##PRE##OP##SUFU); break;    \
case VariableType::BT_UNS_L: objects->addToken(OCODE_##PRE##OP##SUFU); break;  \
                                                                               \
case VariableType::BT_PTR: objects->addToken(OCODE_##PRE##OP##SUFU); break;    \
                                                                               \
default: ERROR_NP("invalid BT: %s", make_string(type->getBasicType()).c_str())

//
// DO_GET_SWITCH
//
#define DO_GET_SWITCH(OP,PRE,SUFF,SUFI,SUFU) \
switch (type->getBasicType()) {DO_GET_CASES(OP, PRE, SUFF, SUFI, SUFU);}

//
// DO_SET_CASES
//
#define DO_SET_CASES(OP,MT,PRE,SUFF,SUFI,SUFU)                                                      \
case VariableType::BT_CHR: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFI); return true;    \
                                                                                                    \
case VariableType::BT_FIX_HH: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFF); return true; \
case VariableType::BT_FIX_H: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFF); return true;  \
case VariableType::BT_FIX: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFF); return true;    \
case VariableType::BT_FIX_L: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFF); return true;  \
case VariableType::BT_FIX_LL: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFF); return true; \
                                                                                                    \
case VariableType::BT_INT_HH: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFI); return true; \
case VariableType::BT_INT_H: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFI); return true;  \
case VariableType::BT_INT: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFI); return true;    \
case VariableType::BT_INT_L: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFI); return true;  \
case VariableType::BT_INT_LL: return false;                                                         \
                                                                                                    \
case VariableType::BT_UNS_HH: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFU); return true; \
case VariableType::BT_UNS_H: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFU); return true;  \
case VariableType::BT_UNS: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFU); return true;    \
case VariableType::BT_UNS_L: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFU); return true;  \
case VariableType::BT_UNS_LL: return false;                                                         \
                                                                                                    \
case VariableType::BT_PTR: objects->addToken(OCODE_##PRE##SETOP_##OP##_##MT##SUFU); return true;    \
                                                                                                    \
default: ERROR_NP("invalid BT: %s", make_string(type->getBasicType()).c_str());

//
// DO_SET_SWITCH
//
#define DO_SET_SWITCH(OP,MT,PRE,SUFF,SUFI,SUFU) \
switch (type->getBasicType()) {DO_SET_CASES(OP, MT, PRE, SUFF, SUFI, SUFU);}

//
// DO_SET_SWITCHES
//
#define DO_SET_SWITCHES(OP,SUFF,SUFI,SUFU,ACS)                                   \
switch (data->type) {                                                            \
case VariableData::MT_AUTO: DO_SET_SWITCH(OP, AUTO, , SUFF, SUFI, SUFU);         \
case VariableData::MT_POINTER: DO_SET_SWITCH(OP, POINTER, , SUFF, SUFI, SUFU);   \
case VariableData::MT_REGISTER:                                                  \
   switch (data->sectionR) {                                                     \
   case VariableData::SR_LOCAL: DO_SET_SWITCH(OP, REGISTER, , SUFF, SUFI, SUFU); \
   case VariableData::SR_MAP: DO_SET_SWITCH(OP, MAPREGISTER, ACS##_, , , );      \
   case VariableData::SR_WORLD: DO_SET_SWITCH(OP, WORLDREGISTER, ACS##_, , , );  \
   case VariableData::SR_GLOBAL: DO_SET_SWITCH(OP, GLOBALREGISTER, ACSE_, , , ); \
   } break;                                                                      \
case VariableData::MT_REGISTERARRAY:                                             \
   switch (data->sectionRA) {                                                    \
   case VariableData::SRA_MAP: DO_SET_SWITCH(OP, MAPARRAY, ACSE_, , , );         \
   case VariableData::SRA_WORLD: DO_SET_SWITCH(OP, WORLDARRAY, ACSE_, , , );     \
   case VariableData::SRA_GLOBAL: DO_SET_SWITCH(OP, GLOBALARRAY, ACSE_, , , );   \
   } break;                                                                      \
case VariableData::MT_STATIC: DO_SET_SWITCH(OP, STATIC, , SUFF, SUFI, SUFU);     \
default: ERROR_NP("invalid MT"); }

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

   virtual void doGet(ObjectVector *objects, VariableType *type, int tmpBase);

   void doGetBaseILLAS(ObjectVector *objects, VariableType *type, int tmpBase,
                       bool add);

   void doGetBaseILLB(ObjectVector *objects, VariableType *type, int tmpBase,
                      ObjectCode ocode);

   virtual bool doSet(ObjectVector *objects, VariableData *data,
                      VariableType *type, int tmpBase);

   SourceExpression::Pointer exprL;
   SourceExpression::Pointer exprR;

   bool assign;

private:
   void doGetBase(ObjectVector *objects, VariableData *dst);
   void doSetBase(ObjectVector *objects, VariableData *dst);
   void doSetBaseGet(ObjectVector *objects, VariableData *src);
   void doSetBaseSet(ObjectVector *objects, VariableData *src);

   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);
};

#endif//HPP_Binary__SourceExpression_

