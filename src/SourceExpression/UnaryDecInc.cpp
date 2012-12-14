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

//=========================================================
// Generics
//

//
// DO_OCODE_VALUE
//
#define DO_OCODE_VALUE(MT,SUF) \
   if(value) \
      ocode = inc ? OCODE_ADD_##MT##_##SUF : OCODE_SUB_##MT##_##SUF; \
   else \
      ocode = inc ? OCODE_INC_##MT##_##SUF : OCODE_DEC_##MT##_##SUF

//=========================================================
// MT_ARRAY
//

//
// ARRAY_ADDRESS
//
#define ARRAY_ADDRESS(TEMP) \
   if(TEMP) tempA = context->getTempVar(0); \
   \
   if(src->offsetExpr) \
      src->offsetExpr->makeObjects(objects, tmp); \
   else \
      objects->addTokenPushZero(); \
   \
   if(TEMP) objects->addToken(OCODE_SET_TEMP, tempA)

//
// ARRAY_FETCH_LO
//
#define ARRAY_FETCH_LO(MT) \
   objects->addToken(OCODE_GET_TEMP, tempA); \
   objects->addToken(OCODE_GET_##MT, addrL)

//
// ARRAY_GETRESULT_PRE
//
#define ARRAY_GETRESULT_PRE(MT) \
   if(!suf && dst->type != VariableData::MT_VOID) \
   { \
      objects->addToken(OCODE_GET_TEMP, tempA); \
      objects->addToken(OCODE_GET_##MT, addrL); \
   } \
   else ((void)0)

//
// ARRAY_GETRESULT_SUF
//
#define ARRAY_GETRESULT_SUF(MT) \
   if(suf && dst->type != VariableData::MT_VOID) \
   { \
      objects->addToken(OCODE_GET_TEMP, tempA); \
      objects->addToken(OCODE_GET_##MT, addrL); \
   } \
   else ((void)0)

//
// ARRAY_GETRESULT2_PRE
//
#define ARRAY_GETRESULT2_PRE(MT,DEC) \
   if(!suf && dst->type != VariableData::MT_VOID) \
   { \
      if(DEC) objects->addToken(OCODE_DEC_TEMP_U, tempA); \
      objects->addToken(OCODE_GET_TEMP, tempA); \
      objects->addToken(OCODE_GET_##MT, addrL); \
      objects->addToken(OCODE_INC_TEMP_U, tempA); \
      objects->addToken(OCODE_GET_TEMP, tempA); \
      objects->addToken(OCODE_GET_##MT, addrL); \
   } \
   else ((void)0)

//
// ARRAY_GETRESULT2_SUF
//
#define ARRAY_GETRESULT2_SUF(MT,INC) \
   if(suf && dst->type != VariableData::MT_VOID) \
   { \
      objects->addToken(OCODE_GET_TEMP, tempA); \
      objects->addToken(OCODE_GET_##MT, addrL); \
      objects->addToken(OCODE_INC_TEMP_U, tempA); \
      objects->addToken(OCODE_GET_TEMP, tempA); \
      if(!INC) objects->addToken(OCODE_DEC_TEMP_U, tempA); \
      objects->addToken(OCODE_GET_##MT, addrL); \
   } \
   else if(INC) \
      objects->addToken(OCODE_INC_TEMP_U, tempA)

//
// ARRAY_MODIFY_LO
//
#define ARRAY_MODIFY_LO() \
   objects->addToken(OCODE_GET_TEMP, tempA); \
   objects->addToken(ocode, addrL)

//
// ARRAY_MODIFY_HI
//
#define ARRAY_MODIFY_HI() \
   objects->addToken(OCODE_GET_TEMP, tempA); \
   objects->addToken(OCODE_GET_IMM, objects->getValue(1)); \
   objects->addToken(OCODE_ADD_STK_U); \
   objects->addToken(ocode, addrL)

//
// DO_L_ARRAY
//
#define DO_L_ARRAY(MT) \
   ARRAY_ADDRESS(true); \
   \
   ocode = inc ? OCODE_INC_##MT##_I : OCODE_DEC_##MT##_I; \
   \
   ARRAY_GETRESULT2_SUF(MT, false); \
   \
   if(inc) {ARRAY_MODIFY_LO();} \
   ARRAY_FETCH_LO(MT); \
   objects->addToken(OCODE_JMP_TRU, objects->getValue(labelEnd)); \
   ARRAY_MODIFY_HI(); \
   objects->addLabel(labelEnd); \
   if(!inc) {ARRAY_MODIFY_LO();} \
   \
   ARRAY_GETRESULT2_PRE(MT, false); \
   \
   break

//
// DO_P_ARRAY
//
#define DO_P_ARRAY(MT,SUF) \
   DO_OCODE_VALUE(MT, U); \
   \
   ARRAY_ADDRESS(dst->type != VariableData::MT_VOID); \
   \
   ARRAY_GETRESULT_SUF(MT); \
   \
   if(dst->type != VariableData::MT_VOID) \
      objects->addToken(OCODE_GET_TEMP, tempA); \
   if(value) objects->addToken(OCODE_GET_IMM, value); \
   objects->addToken(ocode, addrL); \
   \
   ARRAY_GETRESULT_PRE(MT); \
   \
   break

//
// DO_PF_ARRAY
//
#define DO_PF_ARRAY(MT) \
   DO_OCODE_VALUE(MT, U); \
   \
   ARRAY_ADDRESS(true); \
   \
   ARRAY_GETRESULT2_SUF(MT, true); \
   \
   objects->addToken(OCODE_GET_TEMP, tempA); \
   if(value) objects->addToken(OCODE_GET_IMM, value); \
   objects->addToken(ocode, addrL); \
   \
   ARRAY_GETRESULT2_PRE(MT, true); \
   \
   break

//=========================================================
// MT_FARPTR
//

//
// FARPTR_ADDRESS
//
#define FARPTR_ADDRESS() \
   tempA = context->getTempVar(0); \
   tempB = context->getTempVar(1); \
   \
   if(src->offsetExpr) \
      src->offsetExpr->makeObjects(objects, tmp); \
   else \
   { \
      objects->addTokenPushZero(); \
      objects->addTokenPushZero(); \
   } \
   \
   objects->addToken(OCODE_SET_TEMP, tempB); \
   objects->addToken(OCODE_SET_TEMP, tempA)

//
// FARPTR_ADDRESS2
//
#define FARPTR_ADDRESS2() \
   addrH = objects->getValueAdd(addrL, 1); \
   FARPTR_ADDRESS()

//
// FARPTR_FETCH
//
#define FARPTR_FETCH(MT,ADDR) \
   objects->addToken(OCODE_GET_TEMP, tempA); \
   objects->addToken(OCODE_GET_TEMP, tempB); \
   objects->addToken(OCODE_GET_##MT, ADDR)

//
// FARPTR_GETRESULT
//
#define FARPTR_GETRESULT(MT,SUF) \
   if((SUF) && dst->type != VariableData::MT_VOID) \
   { objects->addToken(OCODE_GET_TEMP, tempC); }

//
// FARPTR_GETRESULT2
//
#define FARPTR_GETRESULT2(MT,SUF) \
   if((SUF) && dst->type != VariableData::MT_VOID) \
   { \
      objects->addToken(OCODE_GET_TEMP, tempC); \
      objects->addToken(OCODE_GET_TEMP, tempD); \
   }

//
// FARPTR_STORE
//
#define FARPTR_STORE(MT,ADDR) \
   objects->addToken(OCODE_GET_TEMP, tempA); \
   objects->addToken(OCODE_GET_TEMP, tempB); \
   objects->addToken(OCODE_SET_##MT, ADDR)

//
// DO_L_FARPTR
//
#define DO_L_FARPTR(MT) \
   ocode = inc ? OCODE_INC_TEMP_U : OCODE_DEC_TEMP_U; \
   \
   FARPTR_ADDRESS2(); \
   \
   /* Fetch value. */ \
   FARPTR_FETCH(MT, addrL); \
   objects->addToken(OCODE_SET_TEMP, tempC = context->getTempVar(2)); \
   FARPTR_FETCH(MT, addrH); \
   objects->addToken(OCODE_SET_TEMP, tempD = context->getTempVar(3)); \
   \
   FARPTR_GETRESULT2(MT, suf); \
   \
   /* Modify value. */ \
   if(inc) objects->addToken(ocode, tempC); \
   objects->addToken(OCODE_GET_TEMP, tempC); \
   objects->addToken(OCODE_JMP_TRU, objects->getValue(labelEnd)); \
   objects->addToken(ocode, tempD); \
   objects->addLabel(labelEnd); \
   if(!inc) objects->addToken(ocode, tempC); \
   \
   /* Store value. */ \
   objects->addToken(OCODE_GET_TEMP, tempC); \
   FARPTR_STORE(MT, addrL); \
   objects->addToken(OCODE_GET_TEMP, tempD); \
   FARPTR_STORE(MT, addrH); \
   \
   FARPTR_GETRESULT2(MT, !suf); \
   \
   break

//
// DO_P_FARPTR
//
#define DO_P_FARPTR(MT, SUF) \
   DO_OCODE_VALUE(TEMP, SUF); \
   \
   FARPTR_ADDRESS(); \
   \
   /* Fetch value. */ \
   FARPTR_FETCH(MT, addrL); \
   objects->addToken(OCODE_SET_TEMP, tempC = context->getTempVar(2)); \
   \
   FARPTR_GETRESULT(MT, suf); \
   \
   /* Modify value. */ \
   if(value) objects->addToken(OCODE_GET_IMM, value); \
   objects->addToken(ocode, tempC); \
   \
   /* Store value. */ \
   objects->addToken(OCODE_GET_TEMP, tempC); \
   FARPTR_STORE(MT, addrL); \
   \
   FARPTR_GETRESULT(MT, !suf); \
   \
   break

//
// DO_PF_FARPTR
//
#define DO_PF_FARPTR(MT) \
   DO_OCODE_VALUE(TEMP, U); \
   \
   FARPTR_ADDRESS2(); \
   \
   /* Fetch value. */ \
   FARPTR_FETCH(MT, addrL); \
   objects->addToken(OCODE_SET_TEMP, tempC = context->getTempVar(2)); \
   FARPTR_FETCH(MT, addrH); \
   objects->addToken(OCODE_SET_TEMP, tempD = context->getTempVar(3)); \
   \
   FARPTR_GETRESULT2(MT, suf); \
   \
   /* Modify value. */ \
   if(value) objects->addToken(OCODE_GET_IMM, value); \
   objects->addToken(ocode, tempD); \
   \
   /* Store value. */ \
   objects->addToken(OCODE_GET_TEMP, tempC); \
   FARPTR_STORE(MT, addrL); \
   objects->addToken(OCODE_GET_TEMP, tempD); \
   FARPTR_STORE(MT, addrH); \
   \
   FARPTR_GETRESULT2(MT, !suf); \
   \
   break

//=========================================================
// MT_POINTER
//

//
// POINTER_ADDRESS
//
#define POINTER_ADDRESS(TEMP) \
   if(TEMP) tempA = context->getTempVar(0); \
   \
   if(src->offsetExpr) \
      src->offsetExpr->makeObjects(objects, tmp); \
   else \
      objects->addTokenPushZero(); \
   \
   if(TEMP) objects->addToken(OCODE_SET_TEMP, tempA)

//
// POINTER_ADDRESS2
//
#define POINTER_ADDRESS2(TEMP) \
   addrH = objects->getValueAdd(addrL, 1); \
   POINTER_ADDRESS(TEMP)

//
// POINTER_FETCH
//
#define POINTER_FETCH(MT, ADDR) \
   objects->addToken(OCODE_GET_TEMP, tempA); \
   objects->addToken(OCODE_GET_##MT, ADDR)

//
// POINTER_GETRESULT
//
#define POINTER_GETRESULT(MT,SUF) \
   if((SUF) && dst->type != VariableData::MT_VOID) \
   { POINTER_FETCH(MT, addrL); }

//
// POINTER_GETRESULT2
//
#define POINTER_GETRESULT2(MT,SUF) \
   if((SUF) && dst->type != VariableData::MT_VOID) \
   { POINTER_FETCH(MT, addrL); POINTER_FETCH(MT, addrH); }

//
// POINTER_GETVALUE_PRE
//
#define POINTER_GETVALUE_PRE() \
   if(value && dst->type == VariableData::MT_VOID) \
      objects->addToken(OCODE_GET_IMM, value)

//
// POINTER_GETVALUE_POST
//
#define POINTER_GETVALUE_POST() \
   if(dst->type != VariableData::MT_VOID) \
   { \
      if(value) objects->addToken(OCODE_GET_IMM, value); \
      objects->addToken(OCODE_GET_TEMP, tempA); \
   }

//
// POINTER_MODIFY
//
#define POINTER_MODIFY(ADDR) \
   objects->addToken(OCODE_GET_TEMP, tempA); \
   objects->addToken(ocode, ADDR)

//
// DO_L_POINTER
//
#define DO_L_POINTER(MT) \
   ocode = inc ? OCODE_INC_##MT##_I : OCODE_DEC_##MT##_I; \
   \
   POINTER_ADDRESS2(true); \
   \
   POINTER_GETRESULT2(MT, suf); \
   \
   if(inc) {POINTER_MODIFY(addrL);} \
   POINTER_FETCH(MT, addrL); \
   objects->addToken(OCODE_JMP_TRU, objects->getValue(labelEnd)); \
   POINTER_MODIFY(addrH); \
   objects->addLabel(labelEnd); \
   if(!inc) {POINTER_MODIFY(addrL);} \
   \
   POINTER_GETRESULT2(MT, !suf); \
   \
   break

//
// DO_P_POINTER
//
#define DO_P_POINTER(MT, SUF) \
   DO_OCODE_VALUE(MT, SUF); \
   \
   POINTER_GETVALUE_PRE(); \
   \
   /* If we don't have to push the result, we only need one copy of the */ \
   /* address, so don't bother with the temp. */ \
   POINTER_ADDRESS(dst->type != VariableData::MT_VOID); \
   \
   POINTER_GETRESULT(MT, suf); \
   \
   POINTER_GETVALUE_POST(); \
   objects->addToken(ocode, addrL); \
   \
   POINTER_GETRESULT(MT, !suf); \
   \
   break

//
// DO_PF_POINTER
//
#define DO_PF_POINTER(MT) \
   DO_OCODE_VALUE(MT, U); \
   \
   POINTER_GETVALUE_PRE(); \
   \
   /* If we don't have to push the result, we only need one copy of the */ \
   /* address, so don't bother with the temp. */ \
   POINTER_ADDRESS2(dst->type != VariableData::MT_VOID); \
   \
   POINTER_GETRESULT2(MT, suf); \
   \
   POINTER_GETVALUE_POST(); \
   objects->addToken(ocode, addrH); \
   \
   POINTER_GETRESULT2(MT, !suf); \
   \
   break

//=========================================================
// MT_REGISTER
//

//
// DO_L_REGISTER
//
#define DO_L_REGISTER(MT) \
   addrH = objects->getValueAdd(addrL, 1); \
   ocode = inc ? OCODE_INC_##MT##_I : OCODE_DEC_##MT##_I; \
   \
   if(suf && dst->type != VariableData::MT_VOID) \
   { \
      objects->addToken(OCODE_GET_##MT, addrL); \
      objects->addToken(OCODE_GET_##MT, addrH); \
   } \
   \
   if(inc) objects->addToken(ocode, addrL); \
   objects->addToken(OCODE_GET_##MT, addrL); \
   objects->addToken(OCODE_JMP_TRU, objects->getValue(labelEnd)); \
   objects->addToken(ocode, addrH); \
   objects->addLabel(labelEnd); \
   if(!inc) objects->addToken(ocode, addrL); \
   \
   if(!suf && dst->type != VariableData::MT_VOID) \
   { \
      objects->addToken(OCODE_GET_##MT, addrL); \
      objects->addToken(OCODE_GET_##MT, addrH); \
   } \
   \
   break


//
// DO_P_REGISTER
//
#define DO_P_REGISTER(MT,SUF) \
   DO_OCODE_VALUE(MT, SUF); \
   \
   if(suf && dst->type != VariableData::MT_VOID) \
      objects->addToken(OCODE_GET_##MT, addrL); \
   \
   if(value) objects->addToken(OCODE_GET_IMM, value); \
   objects->addToken(ocode, addrL); \
   \
   if(!suf && dst->type != VariableData::MT_VOID) \
      objects->addToken(OCODE_GET_##MT, addrL); \
   \
   break

//
// DO_PF_REGISTER
//
#define DO_PF_REGISTER(MT) \
   addrH = objects->getValueAdd(addrL, 1); \
   \
   DO_OCODE_VALUE(MT, U); \
   \
   if(suf && dst->type != VariableData::MT_VOID) \
   { \
      objects->addToken(OCODE_GET_##MT, addrL); \
      objects->addToken(OCODE_GET_##MT, addrH); \
   } \
   \
   if(value) objects->addToken(OCODE_GET_IMM, value); \
   objects->addToken(ocode, addrH); \
   \
   if(!suf && dst->type != VariableData::MT_VOID) \
   { \
      objects->addToken(OCODE_GET_##MT, addrL); \
      objects->addToken(OCODE_GET_##MT, addrH); \
   } \
   \
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

   //
   // isSideEffect
   //
   virtual bool isSideEffect() const
   {
      return true;
   }

private:
   //
   // ::doI
   //
   void doI(ObjectVector *objects, VariableData *dst, VariableData *src, VariableData *tmp)
   {
      ObjectExpression::Pointer addrL = src->address;
      ObjectExpression::Pointer tempA, tempB, tempC;
      ObjectExpression::Pointer value;
      ObjectCode ocode;

      switch (src->type)
      {
      case VariableData::MT_ARRAY:
         switch(src->sectionA)
         {
         case VariableData::SA_MAP:    DO_P_ARRAY(MAPARR, I);
         case VariableData::SA_WORLD:  DO_P_ARRAY(WLDARR, I);
         case VariableData::SA_GLOBAL: DO_P_ARRAY(GBLARR, I);
      }
      break;

      case VariableData::MT_AUTO: DO_P_REGISTER(AUTO, I);

      case VariableData::MT_FARPTR: DO_P_FARPTR(FARPTR, I);

      case VariableData::MT_POINTER: DO_P_POINTER(PTR, I);

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:  DO_P_REGISTER(REG, I);
         case VariableData::SR_MAP:    DO_P_REGISTER(MAPREG, I);
         case VariableData::SR_WORLD:  DO_P_REGISTER(WLDREG, I);
         case VariableData::SR_GLOBAL: DO_P_REGISTER(GBLREG, I);
      }
      break;

      case VariableData::MT_STATIC: DO_P_REGISTER(STATIC, I);

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_STRING:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         Error_NP("invalid MT");
      }
   }

   //
   // ::doL
   //
   void doL(ObjectVector *objects, VariableData *dst, VariableData *src, VariableData *tmp)
   {
      ObjectExpression::Pointer addrL = src->address;
      ObjectExpression::Pointer addrH;
      ObjectExpression::Pointer tempA, tempB, tempC, tempD;
      ObjectCode ocode;

      std::string label = context->makeLabel();
      std::string labelEnd = label + "_end";
      ObjectExpression::Pointer objEnd = objects->getValue(labelEnd);

      switch (src->type)
      {
      case VariableData::MT_ARRAY:
         switch(src->sectionA)
         {
         case VariableData::SA_MAP:    DO_L_ARRAY(MAPARR);
         case VariableData::SA_WORLD:  DO_L_ARRAY(WLDARR);
         case VariableData::SA_GLOBAL: DO_L_ARRAY(GBLARR);
      }
      break;

      case VariableData::MT_AUTO: DO_L_REGISTER(AUTO);

      case VariableData::MT_FARPTR: DO_L_FARPTR(FARPTR);

      case VariableData::MT_POINTER: DO_L_POINTER(PTR);

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:  DO_L_REGISTER(REG);
         case VariableData::SR_MAP:    DO_L_REGISTER(MAPREG);
         case VariableData::SR_WORLD:  DO_L_REGISTER(WLDREG);
         case VariableData::SR_GLOBAL: DO_L_REGISTER(GBLREG);
      }
      break;

      case VariableData::MT_STATIC: DO_L_REGISTER(STATIC);

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_STRING:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         Error_NP("invalid MT");
      }
   }

   //
   // ::doP
   //
   void doP(ObjectVector *objects, VariableData *dst, VariableData *src,
            VariableData *tmp, ObjectExpression *value)
   {
      ObjectExpression::Pointer addrL = src->address;
      ObjectExpression::Pointer tempA, tempB, tempC;
      ObjectCode ocode;

      switch (src->type)
      {
      case VariableData::MT_ARRAY:
         switch(src->sectionA)
         {
         case VariableData::SA_MAP:    DO_P_ARRAY(MAPARR, U);
         case VariableData::SA_WORLD:  DO_P_ARRAY(WLDARR, U);
         case VariableData::SA_GLOBAL: DO_P_ARRAY(GBLARR, U);
      }
      break;

      case VariableData::MT_AUTO: DO_P_REGISTER(AUTO, U);

      case VariableData::MT_FARPTR: DO_P_FARPTR(FARPTR, U);

      case VariableData::MT_POINTER: DO_P_POINTER(PTR, U);

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:  DO_P_REGISTER(REG, U);
         case VariableData::SR_MAP:    DO_P_REGISTER(MAPREG, U);
         case VariableData::SR_WORLD:  DO_P_REGISTER(WLDREG, U);
         case VariableData::SR_GLOBAL: DO_P_REGISTER(GBLREG, U);
      }
      break;

      case VariableData::MT_STATIC: DO_P_REGISTER(STATIC, U);

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_STRING:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         Error_NP("invalid MT");
      }
   }

   //
   // ::doPF
   //
   void doPF(ObjectVector *objects, VariableData *dst, VariableData *src,
             VariableData *tmp, ObjectExpression *value)
   {
      ObjectExpression::Pointer addrL = src->address;
      ObjectExpression::Pointer addrH;
      ObjectExpression::Pointer tempA, tempB, tempC, tempD;
      ObjectCode ocode;

      switch(src->type)
      {
      case VariableData::MT_ARRAY:
         switch(src->sectionA)
         {
         case VariableData::SA_MAP:    DO_PF_ARRAY(MAPARR);
         case VariableData::SA_WORLD:  DO_PF_ARRAY(WLDARR);
         case VariableData::SA_GLOBAL: DO_PF_ARRAY(GBLARR);
      }
      break;

      case VariableData::MT_AUTO: DO_PF_REGISTER(AUTO);

      case VariableData::MT_FARPTR: DO_PF_FARPTR(FARPTR);

      case VariableData::MT_POINTER: DO_PF_POINTER(PTR);

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:  DO_PF_REGISTER(REG);
         case VariableData::SR_MAP:    DO_PF_REGISTER(MAPREG);
         case VariableData::SR_WORLD:  DO_PF_REGISTER(WLDREG);
         case VariableData::SR_GLOBAL: DO_PF_REGISTER(GBLREG);
      }
      break;

      case VariableData::MT_STATIC: DO_PF_REGISTER(STATIC);

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_STRING:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         Error_NP("invalid MT");
      }
   }

   //
   // ::doX
   //
   void doX(ObjectVector *objects, VariableData *dst, VariableData *src, VariableData *tmp)
   {
      ObjectExpression::Pointer addrL = src->address;
      ObjectExpression::Pointer tempA, tempB, tempC;
      ObjectExpression::Pointer value = objects->getValue(1.0);
      ObjectCode ocode;

      switch (src->type)
      {
      case VariableData::MT_ARRAY:
         switch(src->sectionA)
         {
         case VariableData::SA_MAP:    DO_P_ARRAY(MAPARR, X);
         case VariableData::SA_WORLD:  DO_P_ARRAY(WLDARR, X);
         case VariableData::SA_GLOBAL: DO_P_ARRAY(GBLARR, X);
      }
      break;

      case VariableData::MT_AUTO: DO_P_REGISTER(AUTO, X);

      case VariableData::MT_FARPTR: DO_P_FARPTR(FARPTR, X);

      case VariableData::MT_POINTER: DO_P_POINTER(PTR, X);

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:  DO_P_REGISTER(REG, X);
         case VariableData::SR_MAP:    DO_P_REGISTER(MAPREG, X);
         case VariableData::SR_WORLD:  DO_P_REGISTER(WLDREG, X);
         case VariableData::SR_GLOBAL: DO_P_REGISTER(GBLREG, X);
      }
      break;

      case VariableData::MT_STATIC: DO_P_REGISTER(STATIC, X);

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_STRING:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         Error_NP("invalid MT");
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
      VariableData::Pointer   tmp  = VariableData::
         create_stack(src->type == VariableData::MT_FARPTR ? 2 : 1);

      if (dst->type != VariableData::MT_VOID)
         make_objects_memcpy_prep(objects, dst, data, pos);

      if(bt == VariableType::BT_INT_L || bt == VariableType::BT_INT_LL ||
         bt == VariableType::BT_UNS_L || bt == VariableType::BT_UNS_LL)
      {
         doL(objects, dst, src, tmp);
      }
      else if (VariableType::IsTypeInteger(bt))
      {
         doI(objects, dst, src, tmp);
      }
      else if (bt == VariableType::BT_PTR)
      {
         bigsint value = type->getReturn()->getSize(pos);
         if(type->getSize(pos) == 1)
         {
            if(value == 1)
               doP(objects, dst, src, tmp, NULL);
            else
               doP(objects, dst, src, tmp, objects->getValue(value));
         }
         else
         {
            if(value == 1)
               doPF(objects, dst, src, tmp, NULL);
            else
               doPF(objects, dst, src, tmp, objects->getValue(value));
         }
      }
      else if (VariableType::IsTypeFixed(bt))
      {
         doX(objects, dst, src, tmp);
      }
      else
         Error_NP("invalid BT");

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

