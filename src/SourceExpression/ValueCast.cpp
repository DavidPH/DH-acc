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

      // Special case for casting an array to a pointer.
      if (exprType->getBasicType() == VariableType::BT_ARRAY &&
              type->getBasicType() == VariableType::BT_POINTER)
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
// SourceExpression::create_value_cast_reinterpret
//
SRCEXP_EXPRVAL_DEFN(et, cast_reinterpret)
{
   return new SourceExpression_ValueCast
   (VariableType::CAST_REINTERPRET, expr, type, context, pos);
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

   // Can't do compile-time auto*->static*.
   if (thisBT == VariableType::BT_POINTER &&
       thisType->getReturn()->getStoreType() == STORE_STATIC &&
       (exprBT == VariableType::BT_ARRAY || exprBT == VariableType::BT_POINTER) &&
       exprType->getReturn()->getStoreType() == STORE_AUTO)
   {
      return false;
   }

   // Special case for casting an array to a pointer.
   if (exprBT == VariableType::BT_ARRAY &&
       thisBT == VariableType::BT_POINTER)
   {
      return exprRef->canMakeObject();
   }

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
   VariableType::Reference exprType = expr->getType()->getUnqualified();
   VariableType           *thisType = type;

   VariableType::BasicType exprBT = exprType->getBasicType();
   VariableType::BasicType thisBT = thisType->getBasicType();

   // Special case for casting an array to a pointer.
   if (exprBT == VariableType::BT_ARRAY &&
       thisBT == VariableType::BT_POINTER)
   {
      return exprRef->makeObject();
   }

   return make_object_cast(expr->makeObject(), thisType, exprType, pos);
}

//
// SourceExpression_ValueCast::virtual_makeObjects
//
void SourceExpression_ValueCast::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   // Special case for casting an array to a pointer.
   if (expr->getType()->getBasicType() == VariableType::BT_ARRAY &&
      type->getBasicType() == VariableType::BT_POINTER)
   {
      exprRef->makeObjectsCast(objects, dst, type);
   }
   else
      expr->makeObjectsCast(objects, dst, type);
}

// EOF

