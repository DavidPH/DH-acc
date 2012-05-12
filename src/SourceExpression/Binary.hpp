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
#define CONSTRUCTOR_TYPE_VARS                        \
VariableType::Reference typeL = exprL->getType();    \
VariableType::Reference typeR = exprR->getType();    \
                                                     \
VariableType::BasicType btL = typeL->getBasicType(); \
VariableType::BasicType btR = typeR->getBasicType();

//
// CONSTRUCTOR_ARRAY_DECAY
//
#define CONSTRUCTOR_ARRAY_DECAY                                    \
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
#define CONSTRUCTOR_POINTER_PREAMBLE                      \
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
// ASSIGN_ARITHMETIC_VARS
//
#define ASSIGN_ARITHMETIC_VARS                    \
VariableType::Reference typeL = exprL->getType(); \
VariableData::Pointer   src   = exprL->getData(); \
ObjectCode ocodeOp  = OCODE_NONE;                 \
ObjectCode ocodeGet = OCODE_NONE;                 \
int ocodeOpType;                                  \
int ocodeGetType;                                 \
bool ocodeTyped = false;                          \
ocodeOpType = getOcodeType(typeL->getBasicType(), &ocodeGetType);

//
// ASSIGN_BITWISE_VARS
//
#define ASSIGN_BITWISE_VARS                       \
VariableType::Reference typeL = exprL->getType(); \
VariableData::Pointer   src   = exprL->getData(); \
ObjectCode ocodeOp  = OCODE_NONE;                 \
ObjectCode ocodeGet = OCODE_NONE;                 \
int ocodeOpType = 0;                              \
int ocodeGetType;                                 \
bool ocodeTyped = false;                          \
getOcodeType(typeL->getBasicType(), &ocodeGetType);

//
// ASSIGN_GET_OCODE_GET_REGISTER
//
#define ASSIGN_GET_OCODE_GET_REGISTER        \
switch (src->sectionR)                       \
{                                            \
case VariableData::SR_LOCAL:                 \
   ocodeGet = OCODE_GET_REGISTER32F;         \
   ocodeTyped = true;                        \
   break;                                    \
                                             \
case VariableData::SR_MAP:                   \
   ocodeGet = OCODE_ACS_GET_MAPREGISTER;     \
   break;                                    \
                                             \
case VariableData::SR_WORLD:                 \
   ocodeGet = OCODE_ACS_GET_WORLDREGISTER;   \
   break;                                    \
                                             \
case VariableData::SR_GLOBAL:                \
   ocodeGet = OCODE_ACSE_GET_GLOBALREGISTER; \
   break;                                    \
}

//
// ASSIGN_GET_OCODE_GET_ARRAY
//
#define ASSIGN_GET_OCODE_GET_ARRAY        \
switch (src->sectionRA)                   \
{                                         \
case VariableData::SRA_MAP:               \
   ocodeGet = OCODE_ACSE_GET_MAPARRAY;    \
   break;                                 \
                                          \
case VariableData::SRA_WORLD:             \
   ocodeGet = OCODE_ACSE_GET_WORLDARRAY;  \
   break;                                 \
                                          \
case VariableData::SRA_GLOBAL:            \
   ocodeGet = OCODE_ACSE_GET_GLOBALARRAY; \
   break;                                 \
}

//
// ASSIGN_GET_OCODE_GET
//
#define ASSIGN_GET_OCODE_GET                                    \
switch (src->type)                                              \
{                                                               \
case VariableData::MT_AUTO:                                     \
   ocodeGet = OCODE_GET_AUTO32F;                                \
   ocodeTyped = true;                                           \
   break;                                                       \
                                                                \
case VariableData::MT_POINTER:                                  \
   ocodeGet = OCODE_GET_POINTER32F;                             \
   ocodeTyped = true;                                           \
   break;                                                       \
                                                                \
case VariableData::MT_REGISTER:                                 \
   ASSIGN_GET_OCODE_GET_REGISTER                                \
   break;                                                       \
                                                                \
case VariableData::MT_REGISTERARRAY:                            \
   ASSIGN_GET_OCODE_GET_ARRAY                                   \
   break;                                                       \
                                                                \
case VariableData::MT_STATIC:                                   \
   ocodeGet = OCODE_GET_STATIC32F;                              \
   ocodeTyped = true;                                           \
   break;                                                       \
                                                                \
case VariableData::MT_LITERAL:                                  \
case VariableData::MT_STACK:                                    \
case VariableData::MT_VOID:                                     \
case VariableData::MT_NONE:                                     \
   ERROR_NP("invalid MT");                                      \
}                                                               \
                                                                \
if (ocodeTyped)                                                 \
{                                                               \
   ocodeOp  = static_cast<ObjectCode>(ocodeOp  + ocodeOpType);  \
   ocodeGet = static_cast<ObjectCode>(ocodeGet + ocodeGetType); \
}

//
// ASSIGN_GET_OCODE_ARITHMETIC_REGISTER
//
#define ASSIGN_GET_OCODE_ARITHMETIC_REGISTER(OPER)      \
switch (src->sectionR)                                  \
{                                                       \
case VariableData::SR_LOCAL:                            \
   ocodeOp  = OCODE_SETOP_##OPER##_REGISTER32F;         \
   ocodeTyped = true;                                   \
   break;                                               \
                                                        \
case VariableData::SR_MAP:                              \
   ocodeOp  = OCODE_ACS_SETOP_##OPER##_MAPREGISTER;     \
   break;                                               \
                                                        \
case VariableData::SR_WORLD:                            \
   ocodeOp  = OCODE_ACS_SETOP_##OPER##_WORLDREGISTER;   \
   break;                                               \
                                                        \
case VariableData::SR_GLOBAL:                           \
   ocodeOp  = OCODE_ACSE_SETOP_##OPER##_GLOBALREGISTER; \
   break;                                               \
}

//
// ASSIGN_GET_OCODE_ARITHMETIC_ARRAY
//
#define ASSIGN_GET_OCODE_ARITHMETIC_ARRAY(OPER)      \
switch (src->sectionRA)                              \
{                                                    \
case VariableData::SRA_MAP:                          \
   ocodeOp  = OCODE_ACSE_SETOP_##OPER##_MAPARRAY;    \
   break;                                            \
                                                     \
case VariableData::SRA_WORLD:                        \
   ocodeOp  = OCODE_ACSE_SETOP_##OPER##_WORLDARRAY;  \
   break;                                            \
                                                     \
case VariableData::SRA_GLOBAL:                       \
   ocodeOp  = OCODE_ACSE_SETOP_##OPER##_GLOBALARRAY; \
   break;                                            \
}

//
// ASSIGN_GET_OCODE_ARITHMETIC
//
#define ASSIGN_GET_OCODE_ARITHMETIC(OPER)                         \
ocodeOpType = getOcodeType(typeL->getBasicType(), &ocodeGetType); \
                                                                  \
switch (src->type)                                                \
{                                                                 \
case VariableData::MT_AUTO:                                       \
   ocodeOp  = OCODE_SETOP_##OPER##_AUTO32F;                       \
   ocodeTyped = true;                                             \
   break;                                                         \
                                                                  \
case VariableData::MT_POINTER:                                    \
   ocodeOp  = OCODE_SETOP_##OPER##_POINTER32F;                    \
   ocodeTyped = true;                                             \
   break;                                                         \
                                                                  \
case VariableData::MT_REGISTER:                                   \
   ASSIGN_GET_OCODE_ARITHMETIC_REGISTER(OPER)                     \
   break;                                                         \
                                                                  \
case VariableData::MT_REGISTERARRAY:                              \
   ASSIGN_GET_OCODE_ARITHMETIC_ARRAY(OPER)                        \
   break;                                                         \
                                                                  \
case VariableData::MT_STATIC:                                     \
   ocodeOp  = OCODE_SETOP_##OPER##_STATIC32F;                     \
   ocodeTyped = true;                                             \
   break;                                                         \
                                                                  \
case VariableData::MT_LITERAL:                                    \
case VariableData::MT_STACK:                                      \
case VariableData::MT_VOID:                                       \
case VariableData::MT_NONE:                                       \
   ERROR_NP("invalid MT");                                        \
}                                                                 \
                                                                  \
ASSIGN_GET_OCODE_GET

//
// ASSIGN_GET_OCODE_BITWISE_REGISTER
//
#define ASSIGN_GET_OCODE_BITWISE_REGISTER(OPER)         \
switch (src->sectionR)                                  \
{                                                       \
case VariableData::SR_LOCAL:                            \
   ocodeOp  = OCODE_SETOP_##OPER##_REGISTER32;          \
   break;                                               \
                                                        \
case VariableData::SR_MAP:                              \
   ocodeOp  = OCODE_ACSE_SETOP_##OPER##_MAPREGISTER;    \
   break;                                               \
                                                        \
case VariableData::SR_WORLD:                            \
   ocodeOp  = OCODE_ACSE_SETOP_##OPER##_WORLDREGISTER;  \
   break;                                               \
                                                        \
case VariableData::SR_GLOBAL:                           \
   ocodeOp  = OCODE_ACSE_SETOP_##OPER##_GLOBALREGISTER; \
   break;                                               \
}

//
// ASSIGN_GET_OCODE_BITWISE_ARRAY
//
#define ASSIGN_GET_OCODE_BITWISE_ARRAY(OPER)         \
switch (src->sectionRA)                              \
{                                                    \
case VariableData::SRA_MAP:                          \
   ocodeOp  = OCODE_ACSE_SETOP_##OPER##_MAPARRAY;    \
   break;                                            \
                                                     \
case VariableData::SRA_WORLD:                        \
   ocodeOp  = OCODE_ACSE_SETOP_##OPER##_WORLDARRAY;  \
   break;                                            \
                                                     \
case VariableData::SRA_GLOBAL:                       \
   ocodeOp  = OCODE_ACSE_SETOP_##OPER##_GLOBALARRAY; \
   break;                                            \
}

//
// ASSIGN_GET_OCODE_BITWISE
//
#define ASSIGN_GET_OCODE_BITWISE(OPER)        \
switch (src->type)                            \
{                                             \
case VariableData::MT_AUTO:                   \
   ocodeOp  = OCODE_SETOP_##OPER##_AUTO32;    \
   break;                                     \
                                              \
case VariableData::MT_POINTER:                \
   ocodeOp  = OCODE_SETOP_##OPER##_POINTER32; \
   break;                                     \
                                              \
case VariableData::MT_REGISTER:               \
   ASSIGN_GET_OCODE_BITWISE_REGISTER(OPER)    \
   break;                                     \
                                              \
case VariableData::MT_REGISTERARRAY:          \
   ASSIGN_GET_OCODE_BITWISE_ARRAY(OPER)       \
   break;                                     \
                                              \
case VariableData::MT_STATIC:                 \
   ocodeOp  = OCODE_SETOP_##OPER##_STATIC32;  \
   break;                                     \
                                              \
case VariableData::MT_LITERAL:                \
case VariableData::MT_STACK:                  \
case VariableData::MT_VOID:                   \
case VariableData::MT_NONE:                   \
   ERROR_NP("invalid MT");                    \
}                                             \
                                              \
ASSIGN_GET_OCODE_GET

//
// EVALUATE_ARITHMETIC_VARS
//
#define EVALUATE_ARITHMETIC_VARS(OPER)               \
VariableType::Reference type = getType();            \
VariableType::BasicType bt   = type->getBasicType(); \
bigsint                 size = type->getSize(pos);   \
                                                     \
ObjectCode ocode = OCODE_##OPER##32F;                \
                                                     \
VariableData::Pointer src = VariableData::create_stack(size);

//
// EVALUATE_BITWISE_VARS
//
#define EVALUATE_BITWISE_VARS(OPER)                  \
VariableType::Reference type = getType();            \
VariableType::BasicType bt   = type->getBasicType(); \
bigsint                 size = type->getSize(pos);   \
                                                     \
ObjectCode ocode = OCODE_##OPER##32;                 \
                                                     \
VariableData::Pointer src = VariableData::create_stack(size);

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

   SourceExpression_Binary(bool arithmetic, SRCEXP_EXPRBIN_ARGS);

   SourceExpression_Binary(VariableType *castL, VariableType *castR,
                           SRCEXP_EXPRBIN_ARGS);

   SourceExpression_Binary(VariableType *castL, VariableType *castR,
                           bool arithmetic, SRCEXP_EXPRBIN_ARGS);

   void doAssignBase
   (ObjectVector *objects, VariableData *dst, VariableData *src,
    ObjectCode ocodeOp, ObjectCode ocodeGet);

   void doEvaluateBase
   (ObjectVector *objects, VariableData *dst, VariableData *src,
    ObjectCode ocode);

   void doEvaluateBaseLLAS
   (ObjectVector *objects, VariableData *dst, VariableData *src, bool add);

   void doEvaluateBaseLLB
   (ObjectVector *objects, VariableData *dst, VariableData *src,
    ObjectCode ocode);

   int getOcodeType
   (VariableType::BasicType bt, int *ocodeGetType = NULL);

   void recurse_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer exprL;
   SourceExpression::Pointer exprR;

private:
   void doCast();
   void doCast(VariableType *castL, VariableType *castR);

   int arithmetic;
};

#endif /* HPP_Binary__SourceExpression_ */

