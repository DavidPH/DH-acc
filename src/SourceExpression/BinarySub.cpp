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
// SourceExpression handling of "operator -" and "operator -=".
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
// SourceExpression_BinarySub
//
class SourceExpression_BinarySub : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinarySub,
                                   SourceExpression_Binary);

public:
   //
   // ::SourceExpression_BinarySub
   //
   SourceExpression_BinarySub(SRCEXP_EXPRBIN_PARM, bool _assign)
    : Super(SRCEXP_EXPRBIN_PASS, NULL, NULL, _assign)
   {
      CONSTRUCTOR_TYPE_VARS();
      CONSTRUCTOR_ARRAY_DECAY();

      if(btL == VariableType::BT_PTR && btR == VariableType::BT_PTR)
      {
         typeL = typeL->getReturn()->setStorage(STORE_STATIC)->getPointer();
         typeR = typeR->getReturn()->setStorage(STORE_STATIC)->getPointer();

         exprL = create_value_cast_force(exprL, typeL, context, pos);
         exprR = create_value_cast_force(exprR, typeR, context, pos);
      }

      // Type constraints.
      if (btL == VariableType::BT_PTR || btR == VariableType::BT_PTR)
      {
         // Pointer constraints.
         if (btR != VariableType::BT_PTR && !VariableType::is_bt_integer(btR))
            Error_NP("pointer - non-integer");

         if (btL != VariableType::BT_PTR)
            Error_NP("non-pointer - pointer");
      }
      else
      {
         CONSTRAINT_ARITHMETIC("-");
      }

      if (assign && !VariableType::is_bt_arithmetic(btR))
         Error_NP("X -= non-arithmetic");

      CONSTRUCTOR_POINTER_PREAMBLE();

      docast = false;

      if(btL == VariableType::BT_PTR && btR != VariableType::BT_PTR)
      {
         exprR = create_value_cast_implicit(exprR, VariableType::get_bt_uns(), context, pos);
         if(retnSize != 1) exprR = create_binary_mul(exprR, exprSize, context, pos);
      }
   }

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      EVALUATE_OBJECTS();
      return ObjectExpression::create_binary_sub(objL, objR, pos);
   }

protected:
   //
   // ::canDoSet
   //
   virtual bool canDoSet(VariableData *data, VariableType *type) const
   {
      CAN_SET_SWITCHES(SUB);
   }

   //
   // ::doGet
   //
   virtual void doGet(ObjectVector *objects, VariableType *type, int tmpBase)
   {
      switch (type->getBasicType())
      {
         DO_GET_CASES(SUB);

      case VariableType::BT_INT_L:
      case VariableType::BT_INT_LL:
      case VariableType::BT_UNS_L:
      case VariableType::BT_UNS_LL:
         doGetBaseILAS(objects, type, tmpBase, false);
         break;
      }
   }

   //
   // ::doSet
   //
   virtual void doSet(ObjectVector *objects, VariableData *data, VariableType *type, int)
   {
      if(type->getBasicType() == VariableType::BT_PTR)
      {
         StoreType store = type->getReturn()->getStoreType();
         ObjectExpression::Pointer address = data->address;

         if(store == STORE_NONE || store == STORE_STRING)
            address = objects->getValueAdd(address, 1);

         switch(data->type)
         {
         case VariableData::MT_STATIC: objects->addToken(OCODE_SUB_STATIC_U, address); break;
         case VariableData::MT_AUTO: objects->addToken(OCODE_SUB_AUTO_U, address); break;
         case VariableData::MT_POINTER: objects->addToken(OCODE_SUB_PTR_U, address); break;
         case VariableData::MT_REGISTER:
            switch(data->sectionR)
            {
            case VariableData::SR_LOCAL: objects->addToken(OCODE_SUB_REG_U, address); break;
            case VariableData::SR_MAP: objects->addToken(OCODE_SUB_MAPREG_U, address); break;
            case VariableData::SR_WORLD: objects->addToken(OCODE_SUB_WLDREG_U, address); break;
            case VariableData::SR_GLOBAL: objects->addToken(OCODE_SUB_GBLREG_U, address); break;
            }
            break;

         case VariableData::MT_ARRAY:
            switch(data->sectionA)
            {
            case VariableData::SA_MAP: objects->addToken(OCODE_SUB_MAPARR_U, address); break;
            case VariableData::SA_WORLD: objects->addToken(OCODE_SUB_WLDARR_U, address); break;
            case VariableData::SA_GLOBAL: objects->addToken(OCODE_SUB_GBLARR_U, address); break;
            }
            break;

         default:
            Error_NP("invalid MT");
         }

         return;
      }

      DO_SET_SWITCHES(SUB);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_sub
//
SourceExpression::Pointer SourceExpression::create_binary_sub
(SRCEXP_EXPRBIN_PARM)
{
   SourceExpression::Pointer exprL = _exprL;
   SourceExpression::Pointer exprR = _exprR;

   SourceContext *&context = _context;
   SourcePosition const &pos = _pos;

   CONSTRUCTOR_TYPE_VARS();
   CONSTRUCTOR_ARRAY_DECAY();

   SourceExpression::Pointer expr =
      new SourceExpression_BinarySub(exprL, exprR, context, pos, false);

   // Slight hack for pointer arithmetic.
   if (btL == VariableType::BT_PTR && btR == VariableType::BT_PTR)
   {
      expr = create_value_cast_explicit
         (expr, VariableType::get_bt_int(), context, pos);

      bigsint retnSize = typeL->getReturn()->getSize(pos);
      SourceExpression::Pointer retnExpr =
         create_value_int(retnSize, context, pos);

      if (retnSize != 1)
         expr = create_binary_div(expr, retnExpr, context, pos);
   }

   return expr;
}

//
// SourceExpression::create_binary_sub_eq
//
SRCEXP_EXPRBIN_DEFN(sub_eq)
{
   return new SourceExpression_BinarySub(exprL, exprR, context, pos, true);
}

// EOF

