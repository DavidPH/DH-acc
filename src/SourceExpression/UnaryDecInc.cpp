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
objects->addToken(OCODE_GET_LITERAL32I, wrapv); \
objects->addToken(OCODE_CMP_EQ32I);             \
objects->addToken(OCODE_BRANCH_ZERO, objEnd);   \
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
   objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(1));\
   objects->addToken(OCODE_ADD32U);             \
   objects->addToken(OCODE_##CODEG, addrH);     \
}                                               \
                                                \
objects->addToken(OCODE_GET_TEMP, tempA);       \
objects->addToken(ocode, addrL);                \
objects->addToken(OCODE_GET_TEMP, tempA);       \
objects->addToken(OCODE_##CODEG, addrL);        \
objects->addToken(OCODE_GET_LITERAL32I, wrapv); \
objects->addToken(OCODE_CMP_EQ32I);             \
objects->addToken(OCODE_BRANCH_ZERO, objEnd);   \
objects->addToken(OCODE_GET_TEMP, tempA);       \
objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(1));\
objects->addToken(OCODE_ADD32U);                \
objects->addToken(ocode, addrH);                \
objects->addLabel(labelEnd);                    \
                                                \
if (!suf && dst->type != VariableData::MT_VOID) \
{                                               \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_##CODEG, addrL);     \
   objects->addToken(OCODE_GET_TEMP, tempA);    \
   objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(1));\
   objects->addToken(OCODE_ADD32U);             \
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
objects->addToken(OCODE_GET_LITERAL32I, wrapv); \
objects->addToken(OCODE_CMP_EQ32I);             \
objects->addToken(OCODE_BRANCH_ZERO, objEnd);   \
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
objects->addToken(OCODE_GET_LITERAL32I, value); \
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
objects->addToken(OCODE_GET_LITERAL32I, value); \
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
   objects->addToken(OCODE_GET_LITERAL32I, value);\
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
   objects->addToken(OCODE_GET_LITERAL32I, value);\
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
      case VariableData::MT_AUTO:
         DO_P_ADDR(GET_AUTO32F,
             SETOP_ADD_AUTO32F,
             SETOP_SUB_AUTO32F);

      case VariableData::MT_POINTER:
         DO_P_POINTER(GET_POINTER32F,
                SETOP_ADD_POINTER32F,
                SETOP_SUB_POINTER32F);
         break;

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:
            DO_P_ADDR(GET_REGISTER32F,
                SETOP_ADD_REGISTER32F,
                SETOP_SUB_REGISTER32F);

         case VariableData::SR_MAP:
            DO_P_ADDR(ACS_GET_MAPREGISTER,
                ACS_SETOP_ADD_MAPREGISTER,
                ACS_SETOP_SUB_MAPREGISTER);

         case VariableData::SR_WORLD:
            DO_P_ADDR(ACS_GET_WORLDREGISTER,
                ACS_SETOP_ADD_WORLDREGISTER,
                ACS_SETOP_SUB_WORLDREGISTER);

         case VariableData::SR_GLOBAL:
            DO_P_ADDR(ACSE_GET_GLOBALREGISTER,
                ACSE_SETOP_ADD_GLOBALREGISTER,
                ACSE_SETOP_SUB_GLOBALREGISTER);
      }
      break;

      case VariableData::MT_REGISTERARRAY:
         switch (src->sectionRA)
         {
         case VariableData::SRA_MAP:
            DO_P_OFFSET(ACSE_GET_MAPARRAY,
                  ACSE_SETOP_ADD_MAPARRAY,
                  ACSE_SETOP_SUB_MAPARRAY);
            break;

         case VariableData::SRA_WORLD:
            DO_P_OFFSET(ACSE_GET_WORLDARRAY,
                  ACSE_SETOP_ADD_WORLDARRAY,
                  ACSE_SETOP_SUB_WORLDARRAY);
            break;

         case VariableData::SRA_GLOBAL:
            DO_P_OFFSET(ACSE_GET_GLOBALARRAY,
                  ACSE_SETOP_ADD_GLOBALARRAY,
                  ACSE_SETOP_SUB_GLOBALARRAY);
            break;
      }
      break;

      case VariableData::MT_STATIC:
         DO_P_ADDR(GET_STATIC32F,
             SETOP_ADD_STATIC32F,
             SETOP_SUB_STATIC32F);

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
      case VariableData::MT_AUTO:
         DO_I_ADDR(GET_AUTO32I,
             SETOP_INC_AUTO32I,
             SETOP_DEC_AUTO32I);

      case VariableData::MT_POINTER:
         DO_I_OFFSET(GET_POINTER32I,
               SETOP_INC_POINTER32I,
               SETOP_DEC_POINTER32I);
         break;

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:
            DO_I_ADDR(GET_REGISTER32I,
                SETOP_INC_REGISTER32I,
                SETOP_DEC_REGISTER32I);

         case VariableData::SR_MAP:
            DO_I_ADDR(ACS_GET_MAPREGISTER,
                ACS_SETOP_INC_MAPREGISTER,
                ACS_SETOP_DEC_MAPREGISTER);

         case VariableData::SR_WORLD:
            DO_I_ADDR(ACS_GET_WORLDREGISTER,
                ACS_SETOP_INC_WORLDREGISTER,
                ACS_SETOP_DEC_WORLDREGISTER);

         case VariableData::SR_GLOBAL:
            DO_I_ADDR(ACSE_GET_GLOBALREGISTER,
                ACSE_SETOP_INC_GLOBALREGISTER,
                ACSE_SETOP_DEC_GLOBALREGISTER);
      }
      break;

      case VariableData::MT_REGISTERARRAY:
         switch (src->sectionRA)
         {
         case VariableData::SRA_MAP:
            DO_I_OFFSET(ACSE_GET_MAPARRAY,
                  ACSE_SETOP_INC_MAPARRAY,
                  ACSE_SETOP_DEC_MAPARRAY);
            break;

         case VariableData::SRA_WORLD:
            DO_I_OFFSET(ACSE_GET_WORLDARRAY,
                  ACSE_SETOP_INC_WORLDARRAY,
                  ACSE_SETOP_DEC_WORLDARRAY);
            break;

         case VariableData::SRA_GLOBAL:
            DO_I_OFFSET(ACSE_GET_GLOBALARRAY,
                  ACSE_SETOP_INC_GLOBALARRAY,
                  ACSE_SETOP_DEC_GLOBALARRAY);
            break;
      }
      break;

      case VariableData::MT_STATIC:
         DO_I_ADDR(GET_STATIC32I,
             SETOP_INC_STATIC32I,
             SETOP_DEC_STATIC32I);

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
      case VariableData::MT_AUTO:
         DO_LL_ADDR(GET_AUTO32I,
              SETOP_INC_AUTO32I,
              SETOP_DEC_AUTO32I);

      case VariableData::MT_POINTER:
         DO_LL_POINTER(GET_POINTER32I,
                 SETOP_INC_POINTER32I,
                 SETOP_DEC_POINTER32I);
         break;

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:
            DO_LL_ADDR(GET_REGISTER32I,
                 SETOP_INC_REGISTER32I,
                 SETOP_DEC_REGISTER32I);

         case VariableData::SR_MAP:
            DO_LL_ADDR(ACS_GET_MAPREGISTER,
                 ACS_SETOP_INC_MAPREGISTER,
                 ACS_SETOP_DEC_MAPREGISTER);

         case VariableData::SR_WORLD:
            DO_LL_ADDR(ACS_GET_WORLDREGISTER,
                 ACS_SETOP_INC_WORLDREGISTER,
                 ACS_SETOP_DEC_WORLDREGISTER);

         case VariableData::SR_GLOBAL:
            DO_LL_ADDR(ACSE_GET_GLOBALREGISTER,
                 ACSE_SETOP_INC_GLOBALREGISTER,
                 ACSE_SETOP_DEC_GLOBALREGISTER);
      }
      break;

      case VariableData::MT_REGISTERARRAY:
         switch (src->sectionRA)
         {
         case VariableData::SRA_MAP:
            DO_LL_OFFSET(ACSE_GET_MAPARRAY,
                   ACSE_SETOP_INC_MAPARRAY,
                   ACSE_SETOP_DEC_MAPARRAY);
            break;

         case VariableData::SRA_WORLD:
            DO_LL_OFFSET(ACSE_GET_WORLDARRAY,
                   ACSE_SETOP_INC_WORLDARRAY,
                   ACSE_SETOP_DEC_WORLDARRAY);
            break;

         case VariableData::SRA_GLOBAL:
            DO_LL_OFFSET(ACSE_GET_GLOBALARRAY,
                   ACSE_SETOP_INC_GLOBALARRAY,
                   ACSE_SETOP_DEC_GLOBALARRAY);
            break;
      }
      break;

      case VariableData::MT_STATIC:
         DO_LL_ADDR(GET_STATIC32I,
              SETOP_INC_STATIC32I,
              SETOP_DEC_STATIC32I);

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
      case VariableData::MT_AUTO:
         DO_P_ADDR(GET_AUTO32I,
             SETOP_ADD_AUTO32U,
             SETOP_SUB_AUTO32U);

      case VariableData::MT_POINTER:
         DO_P_POINTER(GET_POINTER32I,
                SETOP_ADD_POINTER32U,
                SETOP_SUB_POINTER32U);
         break;

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:
            DO_P_ADDR(GET_REGISTER32I,
                SETOP_ADD_REGISTER32U,
                SETOP_SUB_REGISTER32U);

         case VariableData::SR_MAP:
            DO_P_ADDR(ACS_GET_MAPREGISTER,
                ACS_SETOP_ADD_MAPREGISTER,
                ACS_SETOP_SUB_MAPREGISTER);

         case VariableData::SR_WORLD:
            DO_P_ADDR(ACS_GET_WORLDREGISTER,
                ACS_SETOP_ADD_WORLDREGISTER,
                ACS_SETOP_SUB_WORLDREGISTER);

         case VariableData::SR_GLOBAL:
            DO_P_ADDR(ACSE_GET_GLOBALREGISTER,
                ACSE_SETOP_ADD_GLOBALREGISTER,
                ACSE_SETOP_SUB_GLOBALREGISTER);
      }
      break;

      case VariableData::MT_REGISTERARRAY:
         switch (src->sectionRA)
         {
         case VariableData::SRA_MAP:
            DO_P_OFFSET(ACSE_GET_MAPARRAY,
                  ACSE_SETOP_ADD_MAPARRAY,
                  ACSE_SETOP_SUB_MAPARRAY);
            break;

         case VariableData::SRA_WORLD:
            DO_P_OFFSET(ACSE_GET_WORLDARRAY,
                  ACSE_SETOP_ADD_WORLDARRAY,
                  ACSE_SETOP_SUB_WORLDARRAY);
            break;

         case VariableData::SRA_GLOBAL:
            DO_P_OFFSET(ACSE_GET_GLOBALARRAY,
                  ACSE_SETOP_ADD_GLOBALARRAY,
                  ACSE_SETOP_SUB_GLOBALARRAY);
            break;
      }
      break;

      case VariableData::MT_STATIC:
         DO_P_ADDR(GET_STATIC32I,
             SETOP_ADD_STATIC32U,
             SETOP_SUB_STATIC32U);

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

