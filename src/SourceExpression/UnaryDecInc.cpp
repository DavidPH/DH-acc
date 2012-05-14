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
// SourceExpression handling of "operator ++" and "operator --".
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DO_I_ADDR
//
#define DO_I_ADDR(CODEG, CODEI, CODED)          \
ocode = inc ? OCODE_##CODEI : OCODE_##CODED;    \
                                                \
if (suf && dst->type != VariableData::MT_VOID)  \
   objects->addToken(OCODE_##CODEG, addrL);     \
                                                \
objects->addToken(ocode, addrL);                \
                                                \
if (!suf && dst->type != VariableData::MT_VOID) \
   objects->addToken(OCODE_##CODEG, addrL);     \
break

//
// DO_I_OFFSET
//
#define DO_I_OFFSET(CODEG, CODEI, CODED)        \
if (dst->type != VariableData::MT_VOID)         \
   tempA = context->getTempVar(0);              \
ocode = inc ? OCODE_##CODEI : OCODE_##CODED;    \
                                                \
if (src->offsetExpr)                            \
   src->offsetExpr->makeObjects(objects, tmp);  \
else                                            \
   objects->addTokenPushZero();                 \
if (dst->type != VariableData::MT_VOID)         \
   objects->addToken(OCODE_SET_TEMP, tempA);    \
                                                \
if (suf && dst->type != VariableData::MT_VOID)  \
{                                               \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_##CODEG, addrL);     \
}                                               \
                                                \
if (dst->type != VariableData::MT_VOID)         \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
objects->addToken(ocode, addrL);                \
                                                \
if (!suf && dst->type != VariableData::MT_VOID) \
{                                               \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_##CODEG, addrL);     \
}                                               \
break

//
// DO_LL_ADDR
//
#define DO_LL_ADDR(CODEG, CODEI, CODED)         \
addrH = objects->getValueAdd(addrL, 1);         \
ocode = inc ? OCODE_##CODEI : OCODE_##CODED;    \
                                                \
if (suf && dst->type != VariableData::MT_VOID)  \
{                                               \
   objects->addToken(OCODE_##CODEG, addrL);     \
   objects->addToken(OCODE_##CODEG, addrH);     \
}                                               \
                                                \
objects->addToken(ocode, addrL);                \
objects->addToken(OCODE_##CODEG, addrL);        \
objects->addToken(OCODE_GET_IMM, wrapv);        \
objects->addToken(OCODE_CMP_EQ_I);              \
objects->addToken(OCODE_JMP_NIL, objEnd);       \
objects->addToken(ocode, addrH);                \
objects->addLabel(labelEnd);                    \
                                                \
if (!suf && dst->type != VariableData::MT_VOID) \
{                                               \
   objects->addToken(OCODE_##CODEG, addrL);     \
   objects->addToken(OCODE_##CODEG, addrH);     \
}                                               \
break

//
// DO_LL_OFFSET
//
#define DO_LL_OFFSET(CODEG, CODEI, CODED)       \
addrH = addrL;                                  \
tempA = context->getTempVar(0);                 \
ocode = inc ? OCODE_##CODEI : OCODE_##CODED;    \
                                                \
if (src->offsetExpr)                            \
   src->offsetExpr->makeObjects(objects, tmp);  \
else                                            \
   objects->addTokenPushZero();                 \
objects->addToken(OCODE_SET_TEMP, tempA);       \
                                                \
if (suf && dst->type != VariableData::MT_VOID)  \
{                                               \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_##CODEG, addrL);     \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_GET_IMM, objects->getValue(1));\
   objects->addToken(OCODE_ADD_STK_U);          \
   objects->addToken(OCODE_##CODEG, addrH);     \
}                                               \
                                                \
objects->addToken(OCODE_GET_TEMP, tempA);       \
objects->addToken(ocode, addrL);                \
objects->addToken(OCODE_GET_TEMP, tempA);       \
objects->addToken(OCODE_##CODEG, addrL);        \
objects->addToken(OCODE_GET_IMM, wrapv);        \
objects->addToken(OCODE_CMP_EQ_I);              \
objects->addToken(OCODE_JMP_NIL, objEnd);       \
objects->addToken(OCODE_GET_TEMP, tempA);       \
objects->addToken(OCODE_GET_IMM, objects->getValue(1));\
objects->addToken(OCODE_ADD_STK_U);             \
objects->addToken(ocode, addrH);                \
objects->addLabel(labelEnd);                    \
                                                \
if (!suf && dst->type != VariableData::MT_VOID) \
{                                               \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_##CODEG, addrL);     \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_GET_IMM, objects->getValue(1));\
   objects->addToken(OCODE_ADD_STK_U);          \
   objects->addToken(OCODE_##CODEG, addrH);     \
}                                               \
break

//
// DO_LL_POINTER
//
#define DO_LL_POINTER(CODEG, CODEI, CODED)      \
addrH = objects->getValueAdd(addrL, 1);         \
tempA = context->getTempVar(0);                 \
ocode = inc ? OCODE_##CODEI : OCODE_##CODED;    \
                                                \
if (src->offsetExpr)                            \
   src->offsetExpr->makeObjects(objects, tmp);  \
else                                            \
   objects->addTokenPushZero();                 \
objects->addToken(OCODE_SET_TEMP, tempA);       \
                                                \
if (suf && dst->type != VariableData::MT_VOID)  \
{                                               \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_##CODEG, addrL);     \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_##CODEG, addrH);     \
}                                               \
                                                \
objects->addToken(OCODE_GET_TEMP, tempA);       \
objects->addToken(ocode, addrL);                \
objects->addToken(OCODE_GET_TEMP, tempA);       \
objects->addToken(OCODE_##CODEG, addrL);        \
objects->addToken(OCODE_GET_IMM, wrapv);        \
objects->addToken(OCODE_CMP_EQ_I);              \
objects->addToken(OCODE_JMP_NIL, objEnd);       \
objects->addToken(OCODE_GET_TEMP, tempA);       \
objects->addToken(ocode, addrH);                \
objects->addLabel(labelEnd);                    \
                                                \
if (!suf && dst->type != VariableData::MT_VOID) \
{                                               \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_##CODEG, addrL);     \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_##CODEG, addrH);     \
}                                               \
break

//
// DO_P_ADDR
//
#define DO_P_ADDR(CODEG, CODEI, CODED)          \
ocode = inc ? OCODE_##CODEI : OCODE_##CODED;    \
                                                \
if (suf && dst->type != VariableData::MT_VOID)  \
   objects->addToken(OCODE_##CODEG, addrL);     \
                                                \
objects->addToken(OCODE_GET_IMM, value);        \
objects->addToken(ocode, addrL);                \
                                                \
if (!suf && dst->type != VariableData::MT_VOID) \
   objects->addToken(OCODE_##CODEG, addrL);     \
break

//
// DO_P_OFFSET
//
#define DO_P_OFFSET(CODEG, CODEI, CODED)        \
if (dst->type != VariableData::MT_VOID)         \
   tempA = context->getTempVar(0);              \
ocode = inc ? OCODE_##CODEI : OCODE_##CODED;    \
                                                \
if (src->offsetExpr)                            \
   src->offsetExpr->makeObjects(objects, tmp);  \
else                                            \
   objects->addTokenPushZero();                 \
if (dst->type != VariableData::MT_VOID)         \
   objects->addToken(OCODE_SET_TEMP, tempA);    \
                                                \
if (suf && dst->type != VariableData::MT_VOID)  \
{                                               \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_##CODEG, addrL);     \
}                                               \
                                                \
if (dst->type != VariableData::MT_VOID)         \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
objects->addToken(OCODE_GET_IMM, value);        \
objects->addToken(ocode, addrL);                \
                                                \
if (!suf && dst->type != VariableData::MT_VOID) \
{                                               \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_##CODEG, addrL);     \
}                                               \
break

//
// DO_P_POINTER
//
#define DO_P_POINTER(CODEG, CODEI, CODED)       \
if (dst->type != VariableData::MT_VOID)         \
   tempA = context->getTempVar(0);              \
ocode = inc ? OCODE_##CODEI : OCODE_##CODED;    \
                                                \
if (dst->type == VariableData::MT_VOID)         \
   objects->addToken(OCODE_GET_IMM, value);     \
                                                \
if (src->offsetExpr)                            \
   src->offsetExpr->makeObjects(objects, tmp);  \
else                                            \
   objects->addTokenPushZero();                 \
if (dst->type != VariableData::MT_VOID)         \
   objects->addToken(OCODE_SET_TEMP, tempA);    \
                                                \
if (suf && dst->type != VariableData::MT_VOID)  \
{                                               \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_##CODEG, addrL);     \
}                                               \
                                                \
if (dst->type != VariableData::MT_VOID)         \
{                                               \
   objects->addToken(OCODE_GET_IMM, value);     \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
}                                               \
objects->addToken(ocode, addrL);                \
                                                \
if (!suf && dst->type != VariableData::MT_VOID) \
{                                               \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_##CODEG, addrL);     \
}                                               \
break


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_UnaryDecInc
//
class SourceExpression_UnaryDecInc : public SourceExpression_Unary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_UnaryDecInc,
                                   SourceExpression_Unary);

public:
   //
   // ::SourceExpression_UnaryDecInc
   //
   SourceExpression_UnaryDecInc(bool _inc, bool _suf, SRCEXP_EXPRUNA_PARM)
    : Super(SRCEXP_EXPRUNA_PASS), inc(_inc), suf(_suf)
   {
   }

private:
   //
   // ::doF
   //
   void doF
   (ObjectVector *objects, VariableData *dst, VariableData *src,
    VariableData *tmp)
   {
      ObjectExpression::Pointer addrL = src->address;
      ObjectExpression::Pointer tempA;
      ObjectExpression::Pointer value = objects->getValue(1.0);
      ObjectCode ocode;

      switch (src->type)
      {
      case VariableData::MT_STATIC:
         DO_P_ADDR(GET_STATIC, ADD_STATIC_X, SUB_STATIC_X);

      case VariableData::MT_AUTO:
         DO_P_ADDR(GET_AUTO, ADD_AUTO_X, SUB_AUTO_X);

      case VariableData::MT_POINTER:
         DO_P_POINTER(GET_PTR, ADD_PTR_X, SUB_PTR_X);

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:
            DO_P_ADDR(GET_REG, ADD_REG_X, SUB_REG_X);

         case VariableData::SR_MAP:
            DO_P_ADDR(GET_MAPREG, ADD_MAPREG_X, SUB_MAPREG_X);

         case VariableData::SR_WORLD:
            DO_P_ADDR(GET_WLDREG, ADD_WLDREG_X, SUB_WLDREG_X);

         case VariableData::SR_GLOBAL:
            DO_P_ADDR(GET_GBLREG, ADD_GBLREG_X, SUB_GBLREG_X);
      }
      break;

      case VariableData::MT_REGISTERARRAY:
         switch (src->sectionRA)
         {
         case VariableData::SRA_MAP:
            DO_P_OFFSET(GET_MAPARR, ADD_MAPARR_X, SUB_MAPARR_X);

         case VariableData::SRA_WORLD:
            DO_P_OFFSET(GET_WLDARR, ADD_WLDARR_X, SUB_WLDARR_X);

         case VariableData::SRA_GLOBAL:
            DO_P_OFFSET(GET_GBLARR, ADD_GBLARR_X, SUB_GBLARR_X);
      }
      break;

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         ERROR_NP("invalid MT");
      }
   }

   //
   // ::doI
   //
   void doI
   (ObjectVector *objects, VariableData *dst, VariableData *src,
    VariableData *tmp)
   {
      ObjectExpression::Pointer addrL = src->address;
      ObjectExpression::Pointer tempA;
      ObjectCode ocode;

      switch (src->type)
      {
      case VariableData::MT_STATIC:
         DO_I_ADDR(GET_STATIC, INC_STATIC_I, DEC_STATIC_I);

      case VariableData::MT_AUTO:
         DO_I_ADDR(GET_AUTO, INC_AUTO_I, DEC_AUTO_I);

      case VariableData::MT_POINTER:
         DO_I_OFFSET(GET_PTR, INC_PTR_I, DEC_PTR_I);

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:
            DO_I_ADDR(GET_REG, INC_REG_I, DEC_REG_I);

         case VariableData::SR_MAP:
            DO_I_ADDR(GET_MAPREG, INC_MAPREG_I, DEC_MAPREG_I);

         case VariableData::SR_WORLD:
            DO_I_ADDR(GET_WLDREG, INC_WLDREG_I, DEC_WLDREG_I);

         case VariableData::SR_GLOBAL:
            DO_I_ADDR(GET_GBLREG, INC_GBLREG_I, DEC_GBLREG_I);
      }
      break;

      case VariableData::MT_REGISTERARRAY:
         switch (src->sectionRA)
         {
         case VariableData::SRA_MAP:
            DO_I_OFFSET(GET_MAPARR, INC_MAPARR_I, DEC_MAPARR_I);

         case VariableData::SRA_WORLD:
            DO_I_OFFSET(GET_WLDARR, INC_WLDARR_I, DEC_WLDARR_I);

         case VariableData::SRA_GLOBAL:
            DO_I_OFFSET(GET_GBLARR, INC_GBLARR_I, DEC_GBLARR_I);
      }
      break;

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         ERROR_NP("invalid MT");
      }
   }

   //
   // ::doLL
   //
   void doLL
   (ObjectVector *objects, VariableData *dst, VariableData *src,
    VariableData *tmp)
   {
      ObjectExpression::Pointer addrL = src->address;
      ObjectExpression::Pointer addrH;
      ObjectExpression::Pointer tempA;
      ObjectExpression::Pointer wrapv = objects->getValue(inc ? 0 : 0xFFFFFFFF);
      ObjectCode ocode;

      std::string label = context->makeLabel();
      std::string labelEnd = label + "_end";
      ObjectExpression::Pointer objEnd = objects->getValue(labelEnd);

      switch (src->type)
      {
      case VariableData::MT_STATIC:
         DO_LL_ADDR(GET_STATIC, INC_STATIC_I, DEC_STATIC_I);

      case VariableData::MT_AUTO:
         DO_LL_ADDR(GET_AUTO, INC_AUTO_I, DEC_AUTO_I);

      case VariableData::MT_POINTER:
         DO_LL_POINTER(GET_PTR, INC_PTR_I, DEC_PTR_I);

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:
            DO_LL_ADDR(GET_REG, INC_REG_I, DEC_REG_I);

         case VariableData::SR_MAP:
            DO_LL_ADDR(GET_MAPREG, INC_MAPREG_I, DEC_MAPREG_I);

         case VariableData::SR_WORLD:
            DO_LL_ADDR(GET_WLDREG, INC_WLDREG_I, DEC_WLDREG_I);

         case VariableData::SR_GLOBAL:
            DO_LL_ADDR(GET_GBLREG, INC_GBLREG_I, DEC_GBLREG_I);
      }
      break;

      case VariableData::MT_REGISTERARRAY:
         switch (src->sectionRA)
         {
         case VariableData::SRA_MAP:
            DO_LL_OFFSET(GET_MAPARR, INC_MAPARR_I, DEC_MAPARR_I);

         case VariableData::SRA_WORLD:
            DO_LL_OFFSET(GET_WLDARR, INC_WLDARR_I, DEC_WLDARR_I);

         case VariableData::SRA_GLOBAL:
            DO_LL_OFFSET(GET_GBLARR, INC_GBLARR_I, DEC_GBLARR_I);
      }
      break;

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         ERROR_NP("invalid MT");
      }
   }

   //
   // ::doP
   //
   void doP
   (ObjectVector *objects, VariableData *dst, VariableData *src,
    VariableData *tmp, ObjectExpression *value)
   {
      ObjectExpression::Pointer addrL = src->address;
      ObjectExpression::Pointer tempA;
      ObjectCode ocode;

      switch (src->type)
      {
      case VariableData::MT_STATIC:
         DO_P_ADDR(GET_STATIC, ADD_STATIC_U, SUB_STATIC_U);

      case VariableData::MT_AUTO:
         DO_P_ADDR(GET_AUTO, ADD_AUTO_U, SUB_AUTO_U);

      case VariableData::MT_POINTER:
         DO_P_POINTER(GET_PTR, ADD_PTR_U, SUB_PTR_U);

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:
            DO_P_ADDR(GET_REG, ADD_REG_U, SUB_REG_U);

         case VariableData::SR_MAP:
            DO_P_ADDR(GET_MAPREG, ADD_MAPREG_U, SUB_MAPREG_U);

         case VariableData::SR_WORLD:
            DO_P_ADDR(GET_WLDREG, ADD_WLDREG_U, SUB_WLDREG_U);

         case VariableData::SR_GLOBAL:
            DO_P_ADDR(GET_GBLREG, ADD_GBLREG_U, SUB_GBLREG_U);
      }
      break;

      case VariableData::MT_REGISTERARRAY:
         switch (src->sectionRA)
         {
         case VariableData::SRA_MAP:
            DO_P_OFFSET(GET_MAPARR, ADD_MAPARR_U, SUB_MAPARR_U);

         case VariableData::SRA_WORLD:
            DO_P_OFFSET(GET_WLDARR, ADD_WLDARR_U, SUB_WLDARR_U);

         case VariableData::SRA_GLOBAL:
            DO_P_OFFSET(GET_GBLARR, ADD_GBLARR_U, SUB_GBLARR_U);
      }
      break;

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         ERROR_NP("invalid MT");
      }
   }

   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      VariableType::Reference type = getType();
      bigsint                 size = type->getSize(pos);
      VariableType::BasicType bt   = type->getBasicType();
      VariableData::Pointer   src  = expr->getData();
      VariableData::Pointer   data = VariableData::create_stack(size);
      VariableData::Pointer   tmp  = VariableData::create_stack(1);

      if (dst->type != VariableData::MT_VOID)
         make_objects_memcpy_prep(objects, dst, data, pos);

      if (bt == VariableType::BT_INT_LL || bt == VariableType::BT_UNS_LL)
         doLL(objects, dst, src, tmp);
      else if (VariableType::is_bt_integer(bt))
         doI(objects, dst, src, tmp);
      else if (bt == VariableType::BT_PTR)
      {
         bigsint value = type->getReturn()->getSize(pos);
         if (value == 1)
            doI(objects, dst, src, tmp);
         else
            doP(objects, dst, src, tmp, objects->getValue(value));
      }
      else if (VariableType::is_bt_fix(bt))
         doF(objects, dst, src, tmp);
      else
         ERROR_NP("invalid BT");

      if (dst->type != VariableData::MT_VOID)
         make_objects_memcpy_post(objects, dst, data, type, context, pos);
   }

   bool inc;
   bool suf;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_unary_dec_pre
//
SRCEXP_EXPRUNA_DEFN(dec_pre)
{
   return new SourceExpression_UnaryDecInc(false, false, expr, context, pos);
}

//
// SourceExpression::create_unary_dec_suf
//
SRCEXP_EXPRUNA_DEFN(dec_suf)
{
   return new SourceExpression_UnaryDecInc(false, true, expr, context, pos);
}

//
// SourceExpression::create_unary_inc_pre
//
SRCEXP_EXPRUNA_DEFN(inc_pre)
{
   return new SourceExpression_UnaryDecInc(true, false, expr, context, pos);
}

//
// SourceExpression::create_unary_inc_suf
//
SRCEXP_EXPRUNA_DEFN(inc_suf)
{
   return new SourceExpression_UnaryDecInc(true, true, expr, context, pos);
}

// EOF

