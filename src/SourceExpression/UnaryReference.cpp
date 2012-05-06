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
// SourceExpression handling of "operator &".
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_UnaryReference
//
class SourceExpression_UnaryReference : public SourceExpression_Unary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_UnaryReference,
                                   SourceExpression_Unary);

public:
   //
   // ::ourceExpression_UnaryReference
   //
   SourceExpression_UnaryReference(SRCEXP_EXPRUNA_PARM)
    : Super(SRCEXP_EXPRUNA_PASS), type(expr->getType()->getPointer())
   {
   }

   //
   // ::canMakeObject
   //
   virtual bool canMakeObject() const
   {
      VariableData::Pointer data = expr->getData();

      switch (data->type)
      {
      case VariableData::MT_AUTO:
         if (type->getReturn()->getStoreType() == STORE_AUTO)
            return true;
         else
            return false;

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         return false;

      case VariableData::MT_POINTER:
      case VariableData::MT_REGISTERARRAY:
         return !data->offsetExpr || data->offsetExpr->canMakeObject();

      case VariableData::MT_REGISTER:
      case VariableData::MT_STATIC:
         return true;
      }

      return false;
   }

   //
   // ::getType
   //
   virtual VariableType::Reference getType() const
   {
      return type;
   }

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      VariableData::Pointer data = expr->getData();

      switch (data->type)
      {
      case VariableData::MT_AUTO:
         if (type->getReturn()->getStoreType() == STORE_AUTO)
            return data->address;
         else
            return Super::makeObject();

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         break;

      case VariableData::MT_POINTER:
         if (!data->offsetExpr)
            return data->address;

         return ObjectExpression::create_binary_add
                (data->address, data->offsetExpr->makeObject(), pos);

      case VariableData::MT_REGISTER:
      case VariableData::MT_STATIC:
         return data->address;

      case VariableData::MT_REGISTERARRAY:
         if (!data->offsetExpr)
            return ObjectExpression::create_value_int(0, pos);

         return data->offsetExpr->makeObject();
      }

      return Super::makeObject();
   }

private:
   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      VariableData::Pointer data = expr->getData();

      switch (data->type)
      {
      case VariableData::MT_AUTO:
         if (type->getReturn()->getStoreType() == STORE_AUTO)
            objects->addToken(OCODE_GET_LITERAL32I, data->address);
         else
            objects->addToken(OCODE_ADDR_AUTO, data->address);
         break;

      case VariableData::MT_LITERAL:
      case VariableData::MT_STACK:
      case VariableData::MT_VOID:
      case VariableData::MT_NONE:
         ERROR_NP("invalid MT");

      case VariableData::MT_POINTER:
         if (data->offsetExpr)
            data->offsetExpr->makeObjects(objects, dst);
         else
            objects->addTokenPushZero();

         if (!data->address->canResolve() || data->address->resolveInt())
         {
            objects->addToken(OCODE_GET_LITERAL32I, data->address);
            objects->addToken(OCODE_ADD32U);
         }

         break;

      case VariableData::MT_REGISTER:
      case VariableData::MT_STATIC:
         objects->addToken(OCODE_GET_LITERAL32I, data->address);
         break;

      case VariableData::MT_REGISTERARRAY:
         if (data->offsetExpr)
            data->offsetExpr->makeObjects(objects, dst);
         else
            objects->addTokenPushZero();
         break;
      }
   }

   VariableType::Reference type;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_unary_reference
//
SRCEXP_EXPRUNA_DEFN(reference)
{
   return new SourceExpression_UnaryReference(expr, context, pos);
}

// EOF

