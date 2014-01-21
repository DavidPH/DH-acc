//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2014 David Hill
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
// SourceExpression handling of "operator +" and "operator +=".
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
// SourceExpression_BinaryAdd
//
class SourceExpression_BinaryAdd : public SourceExpression_Binary
{
   CounterPreambleNoClone(SourceExpression_BinaryAdd, SourceExpression_Binary);

public:
   //
   // ::SourceExpression_BinaryAdd
   //
   SourceExpression_BinaryAdd(bool _assign, SRCEXP_EXPRBIN_PARM)
    : Super(SRCEXP_EXPRBIN_PASS, NULL, NULL, _assign)
   {
      CONSTRUCTOR_TYPE_VARS();
      CONSTRUCTOR_ARRAY_DECAY();

      // Special case for integer+__string to become __string+integer.
      if(!assign && VariableType::IsTypeInteger(btL) && btR == VariableType::BT_STR)
         swapExpr();

      // Special case for __string+integer.
      if(!assign && btL == VariableType::BT_STR && VariableType::IsTypeInteger(btR))
      {
         docast = false;
         exprR = create_value_cast_implicit(exprR, VariableType::get_bt_uns(), context, pos);
         return;
      }

      // Type constraints.
      if (btL == VariableType::BT_PTR || btR == VariableType::BT_PTR)
      {
         // Pointer constraints.
         if (btL == VariableType::BT_PTR && btR == VariableType::BT_PTR)
            Error_NP("pointer + pointer");

         if (btL == VariableType::BT_PTR && !VariableType::IsTypeInteger(btR))
            Error_NP("pointer + non-integer");

         if (!VariableType::IsTypeInteger(btL) && btR == VariableType::BT_PTR)
            Error_NP("non-integer + pointer");
      }
      else
      {
         CONSTRAINT_ARITHMETIC("+");
      }

      if(assign && !VariableType::IsTypeArithmetic(btR))
         Error_NP("X += non-arithmetic");

      CONSTRUCTOR_POINTER_PREAMBLE();

      docast = false;

      if(btL == VariableType::BT_PTR)
      {
         exprR = create_value_cast_implicit(exprR, VariableType::get_bt_uns(), context, pos);
         if(retnSize != 1) exprR = create_binary_mul(exprR, exprSize, context, pos);
      }

      if(btR == VariableType::BT_PTR)
      {
         exprL = create_value_cast_implicit(exprL, VariableType::get_bt_uns(), context, pos);
         if(retnSize != 1) exprL = create_binary_mul(exprL, exprSize, context, pos);

         // Swap expressions so pointer is always left.
         swapExpr();
      }
   }

   //
   // ::getType
   //
   virtual VariableType::Reference getType() const
   {
      if(!assign && exprL->getType()->getBasicType() == VariableType::BT_STR &&
         exprR->getType()->getBasicType() == VariableType::BT_UNS)
      {
         return VariableType::get_bt_chr()->setQualifier(VariableType::QUAL_CONST)
                                          ->setStorage(STORE_STRING)->getPointer();
      }

      return Super::getType();
   }

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      EVALUATE_OBJECTS();
      return ObjectExpression::create_binary_add(objL, objR, pos);
   }

protected:
   //
   // ::canDoSet
   //
   virtual bool canDoSet(VariableData *data, VariableType *type) const
   {
      if(type->getBasicType() == VariableType::BT_PTR && data->type == VariableData::MT_FARPTR)
         return true;

      CAN_SET_SWITCHES(ADD);
   }

   //
   // ::doGet
   //
   virtual void doGet(ObjectVector *objects, VariableType *type, int tmpBase)
   {
      // STR+UNS will leave a __strptr on the stack as-is.
      if(exprL->getType()->getBasicType() == VariableType::BT_STR &&
         exprR->getType()->getBasicType() == VariableType::BT_UNS)
      {
         return;
      }

      switch (type->getBasicType())
      {
         DO_GET_CASES(ADD);

      case VariableType::BT_FIX_L:
      case VariableType::BT_FIX_LL:
      case VariableType::BT_INT_L:
      case VariableType::BT_INT_LL:
      case VariableType::BT_UNS_L:
      case VariableType::BT_UNS_LL:
         doGetBaseILAS(objects, type, tmpBase, true);
         break;
      }
   }

   //
   // ::doSet
   //
   virtual void doSet(ObjectVector *objects, VariableData *data, VariableType *type, int tmpBase)
   {
      if(type->getBasicType() == VariableType::BT_PTR)
      {
         StoreType store = type->getReturn()->getStoreType();
         ObjectExpression::Pointer address = data->address;

         if(store == STORE_FAR || store == STORE_STRING)
            address = objects->getValueAdd(address, 1);

         switch(data->type)
         {
         case VariableData::MT_FARPTR:
            {
               ObjectExpression::Pointer tmpA = context->getTempVar(tmpBase+0);
               ObjectExpression::Pointer tmpB = context->getTempVar(tmpBase+1);

               objects->addToken(OCODE_SET_TEMP,   tmpB);
               objects->addToken(OCODE_SET_TEMP,   tmpA);

               objects->addToken(OCODE_GET_TEMP,   tmpA);
               objects->addToken(OCODE_GET_TEMP,   tmpB);
               objects->addToken(OCODE_GET_FARPTR, address);

               objects->addToken(OCODE_ADD_STK_U);

               objects->addToken(OCODE_GET_TEMP,   tmpA);
               objects->addToken(OCODE_GET_TEMP,   tmpB);
               objects->addToken(OCODE_SET_FARPTR, address);
            }
            break;

         case VariableData::MT_STATIC: objects->addToken(OCODE_ADD_STATIC_U, address); break;
         case VariableData::MT_AUTO: objects->addToken(OCODE_ADD_AUTO_U, address); break;
         case VariableData::MT_POINTER: objects->addToken(OCODE_ADD_PTR_U, address); break;
         case VariableData::MT_REGISTER:
            switch(data->sectionR)
            {
            case VariableData::SR_LOCAL: objects->addToken(OCODE_ADD_REG_U, address); break;
            case VariableData::SR_MAP: objects->addToken(OCODE_ADD_MAPREG_U, address); break;
            case VariableData::SR_WORLD: objects->addToken(OCODE_ADD_WLDREG_U, address); break;
            case VariableData::SR_GLOBAL: objects->addToken(OCODE_ADD_GBLREG_U, address); break;
            }
            break;

         case VariableData::MT_ARRAY:
            switch(data->sectionA)
            {
            case VariableData::SA_MAP: objects->addToken(OCODE_ADD_MAPARR_U, address); break;
            case VariableData::SA_WORLD: objects->addToken(OCODE_ADD_WLDARR_U, address); break;
            case VariableData::SA_GLOBAL: objects->addToken(OCODE_ADD_GBLARR_U, address); break;
            }
            break;

         default:
            Error_NP("invalid MT");
         }

         return;
      }

      DO_SET_SWITCHES(ADD);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_add
//
SRCEXP_EXPRBIN_DEFN(add)
{
   return new SourceExpression_BinaryAdd(false, exprL, exprR, context, pos);
}

//
// SourceExpression::create_binary_add_eq
//
SRCEXP_EXPRBIN_DEFN(add_eq)
{
   return new SourceExpression_BinaryAdd(true, exprL, exprR, context, pos);
}

// EOF

