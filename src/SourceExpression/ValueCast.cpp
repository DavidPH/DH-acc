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
   SourceExpression_ValueCast
   (VariableType::CastType ct, SourceExpression *_expr, VariableType *_type,
    SRCEXP_EXPR_PARM)
    : Super(SRCEXP_EXPR_PASS), expr(_expr), type(_type->getUnqualified())
   {
      VariableType::Reference exprType = expr->getType();

      if (ct && !(VariableType::get_cast(type, exprType) & ct))
      {
         ERROR_NP("invalid cast: %s to %s",
                 make_string(exprType).c_str(), make_string(type).c_str());
      }

      VariableType::BasicType exprBT = exprType->getBasicType();

      // Special case for casting an array to a pointer.
      // Or for casting a function to a pointer.
      if((exprBT == VariableType::BT_ARR || VariableType::is_bt_function(exprBT)) &&
         type->getBasicType() == VariableType::BT_PTR)
      {
         exprRef = create_unary_reference(expr, context, pos);
      }
   }

   virtual bool canMakeObject() const;

   virtual VariableType::Reference getType() const;

   virtual ObjectExpression::Pointer makeObject() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer expr, exprRef;
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
      SRCEXP_EXPR_PARM) : Super(SRCEXP_EXPR_PASS), expr(_expr), type(_type) {}

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


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_value_cast_explicit
//
SRCEXP_EXPRVAL_DEFN(et, cast_explicit)
{
   return new SourceExpression_ValueCast
   (VariableType::CAST_EXPLICIT, expr, type, context, pos);
}

//
// SourceExpression::create_value_cast_force
//
SRCEXP_EXPRVAL_DEFN(et, cast_force)
{
   return new SourceExpression_ValueCast
   (VariableType::CAST_FORCE, expr, type, context, pos);
}

//
// SourceExpression::create_value_cast_implicit
//
SRCEXP_EXPRVAL_DEFN(et, cast_implicit)
{
   return new SourceExpression_ValueCast
   (VariableType::CAST_IMPLICIT, expr, type, context, pos);
}

//
// SourceExpression::create_value_cast_qualifier
//
SRCEXP_EXPRVAL_DEFN(et, cast_qualifier)
{
   return new SourceExpression_ValueCast
   (VariableType::CAST_QUALIFIER, expr, type, context, pos);
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
   return new SourceExpression_ValueCast
   (VariableType::CAST_STATIC, expr, type, context, pos);
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
   if(thisBT == VariableType::BT_PTR &&
      (exprBT == VariableType::BT_ARR || exprBT == VariableType::BT_PTR))
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

   // Special cast.
   if(exprRef) return exprRef->canMakeObject();

   return expr->canMakeObject();
}

//
// SourceExpression_ValueCast::getType
//
VariableType::Reference SourceExpression_ValueCast::getType() const
{
   return type;
}

//
// SourceExpression_ValueCast::makeObject
//
ObjectExpression::Pointer SourceExpression_ValueCast::makeObject() const
{
   // Special cast.
   if(exprRef) return exprRef->makeObject();

   return make_object_cast(expr->makeObject(), type, expr->getType(), pos);
}

//
// SourceExpression_ValueCast::virtual_makeObjects
//
void SourceExpression_ValueCast::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   // Special cast.
   if(exprRef)
      exprRef->makeObjectsCast(objects, dst, type);
   else
      expr->makeObjectsCast(objects, dst, type);
}

// EOF

