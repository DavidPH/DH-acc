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
// SourceExpression handling of "operator >>" and "operator >>=".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryRSh
//
class SourceExpression_BinaryRSh : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryRSh,
                                   SourceExpression_Binary);

public:
   //
   // ::SourceExpression_BinaryRSh
   //
   SourceExpression_BinaryRSh(bool _assign, SRCEXP_EXPRBIN_PARM)
    : Super(NULL, NULL, SRCEXP_EXPRBIN_PASS), assign(_assign)
   {
      CONSTRUCTOR_TYPE_VARS
      CONSTRUCTOR_ARRAY_DECAY

      CONSTRAINT_INTEGER(">>")

      exprR = create_value_cast_implicit
      (exprR, VariableType::get_bt_uint(), context, pos);
   }

   //
   // ::canMakeObject
   //
   virtual bool canMakeObject() const
   {
      return false;
   }

   //
   // ::getType
   //
   VariableType::Reference getType() const
   {
      return exprL->getType();
   }

private:
   //
   // ::doAssign
   //
   void doAssign(ObjectVector *objects, VariableData *dst)
   {
      ASSIGN_BITWISE_VARS
      VariableType::BasicType bt = typeL->getBasicType();

      if (VariableType::is_bt_unsigned(bt))
         return doAssignU(objects, dst, src, typeL);

      ASSIGN_GET_OCODE_BITWISE(RSH)

      doAssignBase(objects, dst, src, ocodeOp, ocodeGet);
   }

   //
   // ::doAssignU
   //
   void doAssignU
   (ObjectVector *objects, VariableData *dst, VariableData *src,
    VariableType *type)
   {
      ObjectExpression::Pointer tempA;
      ObjectExpression::Pointer tempS = context->getTempVar(0);

      VariableData::Pointer tmp = VariableData::create_stack(src->size);

      std::string label = context->makeLabel();
      std::string labelEnd = label + "_end";

      if (src->type == VariableData::MT_POINTER ||
          src->type == VariableData::MT_REGISTERARRAY)
         tempA = context->getTempVar(1);

      if (dst->type != VariableData::MT_VOID)
         make_objects_memcpy_prep(objects, dst, tmp, pos);

      create_value_cast_implicit(exprR, type, context, pos)
         ->makeObjects(objects, tmp);
      if (tempA)
      {
         if (src->offsetExpr)
            src->offsetExpr->makeObjects(objects, tmp);
         else
            objects->addTokenPushZero();

         objects->addToken(OCODE_SET_TEMP, tempA);
      }
      objects->addToken(OCODE_SET_TEMP, tempS);

      // Fetch exprL.
      switch (src->type)
      {
      case VariableData::MT_AUTO:
         objects->addToken(OCODE_GET_AUTO32I, src->address);
         break;

      case VariableData::MT_POINTER:
         objects->addToken(OCODE_GET_TEMP, tempA);
         objects->addToken(OCODE_GET_POINTER32I, src->address);
         break;

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:
            objects->addToken(OCODE_GET_REGISTER32I, src->address);
            break;

         case VariableData::SR_MAP:
            objects->addToken(OCODE_ACS_GET_MAPREGISTER, src->address);
            break;

         case VariableData::SR_WORLD:
            objects->addToken(OCODE_ACS_GET_WORLDREGISTER, src->address);
            break;

         case VariableData::SR_GLOBAL:
            objects->addToken(OCODE_ACSE_GET_GLOBALREGISTER, src->address);
            break;
         }
      break;

      case VariableData::MT_REGISTERARRAY:
         // Leave an extra address on the stack to set by.
         objects->addToken(OCODE_GET_TEMP, tempA);
         objects->addToken(OCODE_GET_TEMP, tempA);

         switch (src->sectionRA)
         {
         case VariableData::SRA_MAP:
            objects->addToken(OCODE_ACSE_GET_MAPARRAY, src->address);
            break;

         case VariableData::SRA_WORLD:
            objects->addToken(OCODE_ACSE_GET_WORLDARRAY, src->address);
            break;

         case VariableData::SRA_GLOBAL:
            objects->addToken(OCODE_ACSE_GET_GLOBALARRAY, src->address);
            break;
         }
      break;

      case VariableData::MT_STATIC:
         objects->addToken(OCODE_GET_STATIC32I, src->address);
         break;

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         ERROR_NP("invalid MT");
      }

      // Calculate value.
      objects->addToken(OCODE_GET_TEMP, tempS);
      objects->addToken(OCODE_BITWISE_RSH32);
      objects->addToken(OCODE_GET_TEMP, tempS);
      objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(labelEnd));
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0x80000000));
      objects->addToken(OCODE_SETOP_DEC_TEMP, tempS);
      objects->addToken(OCODE_GET_TEMP, tempS);
      objects->addToken(OCODE_BITWISE_RSH32);
      objects->addToken(OCODE_BITWISE_NOT32);
      objects->addToken(OCODE_BITWISE_AND32);

      objects->addLabel(labelEnd);
      // Assign exprL.
      switch (src->type)
      {
      case VariableData::MT_AUTO:
         objects->addToken(OCODE_SET_AUTO32I, src->address);
         if (dst->type != VariableData::MT_VOID)
            objects->addToken(OCODE_GET_AUTO32I, src->address);
         break;

      case VariableData::MT_POINTER:
         objects->addToken(OCODE_GET_TEMP, tempA);
         objects->addToken(OCODE_SET_POINTER32I, src->address);
         if (dst->type != VariableData::MT_VOID)
         {
            objects->addToken(OCODE_GET_TEMP, tempA);
            objects->addToken(OCODE_GET_POINTER32I, src->address);
         }
         break;

      case VariableData::MT_REGISTER:
         switch (src->sectionR)
         {
         case VariableData::SR_LOCAL:
            objects->addToken(OCODE_SET_REGISTER32I, src->address);
            if (dst->type != VariableData::MT_VOID)
               objects->addToken(OCODE_GET_REGISTER32I, src->address);
            break;

         case VariableData::SR_MAP:
            objects->addToken(OCODE_ACS_SET_MAPREGISTER, src->address);
            if (dst->type != VariableData::MT_VOID)
            objects->addToken(OCODE_ACS_SET_MAPREGISTER, src->address);
            break;

         case VariableData::SR_WORLD:
            objects->addToken(OCODE_ACS_SET_WORLDREGISTER, src->address);
            if (dst->type != VariableData::MT_VOID)
            break;

         case VariableData::SR_GLOBAL:
            objects->addToken(OCODE_ACSE_SET_GLOBALREGISTER, src->address);
            if (dst->type != VariableData::MT_VOID)
            break;
         }
      break;

      case VariableData::MT_REGISTERARRAY:
         // Address already on stack.
         switch (src->sectionRA)
         {
         case VariableData::SRA_MAP:
            objects->addToken(OCODE_ACSE_SET_MAPARRAY, src->address);
            break;

         case VariableData::SRA_WORLD:
            objects->addToken(OCODE_ACSE_SET_WORLDARRAY, src->address);
            break;

         case VariableData::SRA_GLOBAL:
            objects->addToken(OCODE_ACSE_SET_GLOBALARRAY, src->address);
            break;
         }

         if (dst->type != VariableData::MT_VOID)
         {
            objects->addToken(OCODE_GET_TEMP, tempA);
            switch (src->sectionRA)
            {
            case VariableData::SRA_MAP:
               objects->addToken(OCODE_ACSE_GET_MAPARRAY, src->address);
               break;

            case VariableData::SRA_WORLD:
               objects->addToken(OCODE_ACSE_GET_WORLDARRAY, src->address);
               break;

            case VariableData::SRA_GLOBAL:
               objects->addToken(OCODE_ACSE_GET_GLOBALARRAY, src->address);
               break;
            }
         }
      break;

      case VariableData::MT_STATIC:
         objects->addToken(OCODE_SET_STATIC32I, src->address);
         if (dst->type != VariableData::MT_VOID)
            objects->addToken(OCODE_GET_STATIC32I, src->address);
         break;

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         ERROR_NP("invalid MT");
      }

      if (dst->type != VariableData::MT_VOID)
         make_objects_memcpy_post(objects, dst, tmp, type, context, pos);
   }

   //
   // ::doEvaluate
   //
   void doEvaluate(ObjectVector *objects, VariableData *dst)
   {
      EVALUATE_BITWISE_VARS(BITWISE_RSH)

      if (VariableType::is_bt_unsigned(bt))
         return doEvaluateU(objects, dst, src, type);

      doEvaluateBase(objects, dst, src, ocode);
   }

   //
   // ::doEvaluateU
   //
   void doEvaluateU
   (ObjectVector *objects, VariableData *dst, VariableData *src,
    VariableType *type)
   {
      ObjectExpression::Pointer tempS = context->getTempVar(0);

      std::string label = context->makeLabel();
      std::string labelEnd = label + "_end";

      make_objects_memcpy_prep(objects, dst, src, pos);

      create_value_cast_implicit(exprL, type, context, pos)
         ->makeObjects(objects, src);

      create_value_cast_implicit(exprR, type, context, pos)
         ->makeObjects(objects, src);

      objects->addToken(OCODE_SET_TEMP, tempS);
      objects->addToken(OCODE_GET_TEMP, tempS);
      objects->addToken(OCODE_BITWISE_RSH32);
      objects->addToken(OCODE_GET_TEMP, tempS);
      objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(labelEnd));
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0x80000000));
      objects->addToken(OCODE_SETOP_DEC_TEMP, tempS);
      objects->addToken(OCODE_GET_TEMP, tempS);
      objects->addToken(OCODE_BITWISE_RSH32);
      objects->addToken(OCODE_BITWISE_NOT32);
      objects->addToken(OCODE_BITWISE_AND32);

      objects->addLabel(labelEnd);
      make_objects_memcpy_post(objects, dst, src, type, context, pos);
   }

   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      if (assign)
         doAssign(objects, dst);
      else
         doEvaluate(objects, dst);
   }

   bool assign;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_rsh
//
SRCEXP_EXPRBIN_DEFN(rsh)
{
   return new SourceExpression_BinaryRSh(false, exprL, exprR, context, pos);
}

//
// SourceExpression::create_binary_rsh_eq
//
SRCEXP_EXPRBIN_DEFN(rsh_eq)
{
   return new SourceExpression_BinaryRSh(true, exprL, exprR, context, pos);
}

// EOF

