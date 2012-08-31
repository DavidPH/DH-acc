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
// SourceExpression handling of type-casting.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../SourceException.hpp"
#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_ValueCast
//
class SourceExpression_ValueCast : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_ValueCast,
                                   SourceExpression);

public:
   //
   // ::SourceExpression_ValueCast
   //
   SourceExpression_ValueCast(VariableType::CastType cast, SourceExpression *_expr,
                              VariableType *_type, SRCEXP_EXPR_PARM)
    : Super(SRCEXP_EXPR_PASS), expr(_expr), type(_type->getUnqualified())
   {
      VariableType::Reference exprType = expr->getType();

      VariableType::BasicType exprBT = exprType->getBasicType();
      VariableType::BasicType thisBT = type->getBasicType();

      ObjectExpression::Pointer obj;

      // array->pointer
      if(thisBT == VariableType::BT_PTR && exprBT == VariableType::BT_ARR)
      {
         exprType = exprType->getReturn()->getPointer();

         expr = create_unary_reference(expr, context, pos);
         expr = create_value_cast_force(expr, exprType, context, pos);
         exprType = expr->getType();
         exprBT = exprType->getBasicType();
      }

      // function->pointer
      if(thisBT == VariableType::BT_PTR && VariableType::is_bt_function(exprBT))
      {
         expr = create_unary_reference(expr, context, pos);
         exprType = expr->getType();
         exprBT = exprType->getBasicType();
      }

      // 0->pointer
      if(thisBT == VariableType::BT_PTR && VariableType::is_bt_integer(exprBT) &&
         expr->canMakeObject() && (obj = expr->makeObject())->canResolve())
      {
         if(obj->getType() == ObjectExpression::ET_INT && obj->resolveINT() == 0)
            return;

         if(obj->getType() == ObjectExpression::ET_UNS && obj->resolveUNS() == 0)
            return;
      }

      // pointer->auto pointer
      if(thisBT == VariableType::BT_PTR && exprBT == VariableType::BT_PTR &&
         type->getReturn()->getStoreType() == STORE_AUTO)
      {
         type = type->getReturn()->setStorage(exprType->getReturn())->getPointer();
      }

      if(VariableType::get_cast(type, exprType) > cast)
      {
         Error_NP("invalid cast: %s to %s",
                 make_string(exprType).c_str(), make_string(type).c_str());
      }
   }

   virtual bool canMakeObject() const;

   //
   // ::getType
   //
   virtual VariableType::Reference getType() const {return type;}

   //
   // ::makeObject
   //
   ObjectExpression::Pointer makeObject() const
   {
      return make_object_cast(expr->makeObject(), type, expr->getType(), pos);
   }

private:
   //
   // ::virtual_makeObjects
   //
   void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      expr->makeObjectsCast(objects, dst, type);
   }

   SourceExpression::Pointer expr;
   VariableType::Reference type;
};

//
// SourceExpression_ValueCastQualifier
//
class SourceExpression_ValueCastQualifier : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_ValueCastQualifier, SourceExpression);

public:
   //
   // ::SourceExpression_ValueCastQualifier
   //
   SourceExpression_ValueCastQualifier(SourceExpression *_expr, VariableType *_type,
                                       SRCEXP_EXPR_PARM)
    : Super(SRCEXP_EXPR_PASS), expr(_expr), type(_type->getUnqualified())
   {
      VariableType::Reference exprType = expr->getType();
      VariableType::BasicType exprBT = exprType->getBasicType();
      VariableType::BasicType thisBT = type->getBasicType();
      VariableType::Reference exprRetn = exprType->getReturn();
      VariableType::Reference thisRetn = type->getReturn();

      if(exprBT != VariableType::BT_PTR || thisBT != VariableType::BT_PTR ||
         exprRetn->getStoreType() != thisRetn->getStoreType() ||
         exprRetn->getStoreArea() != thisRetn->getStoreArea() ||
         exprRetn->getUnqualified() != thisRetn->getUnqualified())
      {
         Error_NP("invalid const_cast: %s to %s",
                  make_string(exprType).c_str(), make_string(type).c_str());
      }
   }

   //
   // ::canMakeObject
   //
   virtual bool canMakeObject() const {return expr->canMakeObject();}

   //
   // ::getType
   //
   virtual VariableType::Reference getType() const {return type;}

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const {return expr->makeObject();}

private:
   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      expr->makeObjects(objects, dst);
   }

   SourceExpression::Pointer expr;
   VariableType::Reference type;
};

//
// SourceExpression_ValueCastRaw
//
class SourceExpression_ValueCastRaw : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_ValueCastRaw, SourceExpression);

public:
   //
   // ::SourceExpression_ValueCastRaw
   //
   SourceExpression_ValueCastRaw(SourceExpression *_expr, VariableType *_type,
                                 SRCEXP_EXPR_PARM)
    : Super(SRCEXP_EXPR_PASS), expr(_expr), type(_type->getUnqualified())
   {
   }

   //
   // ::canMakeObject
   //
   virtual bool canMakeObject() const {return false;}

   //
   // ::getType
   //
   virtual VariableType::Reference getType() const {return type;}

private:
   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);


      biguint exprSize = expr->getType()->getSize(pos), size = type->getSize(pos);

      VariableData::Pointer tmp = VariableData::create_stack(size);


      make_objects_memcpy_prep(objects, dst, tmp, pos);

      expr->makeObjects(objects, VariableData::create_stack(exprSize));

      for(; exprSize < size; ++exprSize) objects->addTokenPushZero();
      for(; exprSize > size; --exprSize) objects->addToken(OCODE_STK_DROP);

      make_objects_memcpy_post(objects, dst, tmp, type, context, pos);
   }

   SourceExpression::Pointer expr;
   VariableType::Reference type;
};

//
// SourceExpression_ValueCastStorage
//
class SourceExpression_ValueCastStorage : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_ValueCastStorage, SourceExpression);

public:
   //
   // ::SourceExpression_ValueCastStorage
   //
   SourceExpression_ValueCastStorage(SourceExpression *_expr, VariableType *_type,
                                       SRCEXP_EXPR_PARM)
    : Super(SRCEXP_EXPR_PASS), expr(_expr), type(_type->getUnqualified())
   {
      VariableType::Reference exprType = expr->getType();
      VariableType::BasicType exprBT = exprType->getBasicType();
      VariableType::BasicType thisBT = type->getBasicType();
      VariableType::Reference exprRetn = exprType->getReturn();
      VariableType::Reference thisRetn = type->getReturn();

      if(exprBT != VariableType::BT_PTR || thisBT != VariableType::BT_PTR ||
         exprRetn->getQualifiers() != thisRetn->getQualifiers() ||
         exprRetn->getUnqualified() != thisRetn->getUnqualified())
      {
         Error_NP("invalid store_cast: %s to %s",
                  make_string(exprType).c_str(), make_string(type).c_str());
      }
   }

   //
   // ::canMakeObject
   //
   virtual bool canMakeObject() const {return expr->canMakeObject();}

   //
   // ::getType
   //
   virtual VariableType::Reference getType() const {return type;}

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      return make_object_cast(expr->makeObject(), type, expr->getType(), pos);
   }

private:
   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      expr->makeObjectsCast(objects, dst, type);
   }

   SourceExpression::Pointer expr;
   VariableType::Reference type;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_value_cast_explicit
//
SRCEXP_EXPRVAL_DEFN(et, cast_explicit)
{
   return new SourceExpression_ValueCast(VariableType::CAST_FORCE, expr, type, context, pos);
}

//
// SourceExpression::create_value_cast_force
//
SRCEXP_EXPRVAL_DEFN(et, cast_force)
{
   return new SourceExpression_ValueCast(VariableType::CAST_NEVER, expr, type, context, pos);
}

//
// SourceExpression::create_value_cast_implicit
//
SRCEXP_EXPRVAL_DEFN(et, cast_implicit)
{
   return new SourceExpression_ValueCast(VariableType::CAST_CONVE, expr, type, context, pos);
}

//
// SourceExpression::create_value_cast_qualifier
//
SRCEXP_EXPRVAL_DEFN(et, cast_qualifier)
{
   return new SourceExpression_ValueCastQualifier(expr, type, context, pos);
}

//
// SourceExpression::create_value_cast_raw
//
SRCEXP_EXPRVAL_DEFN(et, cast_raw)
{
   return new SourceExpression_ValueCastRaw(expr, type, context, pos);
}

//
// SourceExpression::create_value_cast_static
//
SRCEXP_EXPRVAL_DEFN(et, cast_static)
{
   return new SourceExpression_ValueCast(VariableType::CAST_CONVE, expr, type, context, pos);
}

//
// SourceExpression::create_value_cast_storage
//
SRCEXP_EXPRVAL_DEFN(et, cast_storage)
{
   return new SourceExpression_ValueCastStorage(expr, type, context, pos);
}

//
// SourceExpression_ValueCast::canMakeObject
//
bool SourceExpression_ValueCast::canMakeObject() const
{
   VariableType::Reference exprType = expr->getType();
   VariableType::Reference thisType = type;

   VariableType::BasicType exprBT = exprType->getBasicType();
   VariableType::BasicType thisBT = thisType->getBasicType();

   // Special pointer considerations.
   if(thisBT == VariableType::BT_PTR && exprBT == VariableType::BT_PTR)
   {
      StoreType exprST = exprType->getReturn()->getStoreType();
      StoreType thisST = thisType->getReturn()->getStoreType();

      // Can't do compile-time auto*->static*.
      if(thisST == STORE_STATIC && exprST == STORE_AUTO)
         return false;

      // Can't do compile-time auto*->far*.
      if(thisST == STORE_NONE && exprST == STORE_AUTO)
         return false;

      // Can't do compile-time string*->far*.
      if(thisST == STORE_NONE && exprST == STORE_STRING)
         return false;
   }

   // Can't do compile-time __string->far*.
   if(thisBT == VariableType::BT_PTR && exprBT == VariableType::BT_STR &&
      thisType->getReturn()->getStoreType() == STORE_NONE)
      return false;

   return expr->canMakeObject();
}

// EOF

