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
   SourceExpression_ValueCast
   (SourceExpression *expr, VariableType *type, bool implicit,
    SRCEXP_EXPR_ARGS);

   virtual bool canMakeObject() const;

   virtual VariableType::Reference getType() const;

   virtual ObjectExpression::Pointer makeObject() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

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
   return new SourceExpression_ValueCast(expr, type, false, context, position);
}

//
// SourceExpression::create_value_cast_implicit
//
SRCEXP_EXPRVAL_DEFN(et, cast_implicit)
{
   return new SourceExpression_ValueCast(expr, type, true, context, position);
}

//
// SourceExpression_ValueCast::SourceExpression_ValueCast
//
SourceExpression_ValueCast::SourceExpression_ValueCast
(SourceExpression *_expr, VariableType *_type, bool implicit, SRCEXP_EXPR_PARM)
 : Super(SRCEXP_EXPR_PASS), expr(_expr), type(_type->getUnqualified())
{
   #define BAD_CAST()                                           \
   if (true)                                                    \
   {                                                            \
      exceptString  = "invalid implicit cast (";                \
      exceptString += make_string(exprBT);                      \
      exceptString += " to ";                                   \
      exceptString += make_string(thisBT);                      \
      exceptString += ")";                                      \
      throw SourceException(exceptString, position, getName()); \
   }                                                            \
   else (void)0

   // Explicit casts bypass all tests. (For now.)
   if (!implicit) return;

   std::string exceptString;

   VariableType::Reference exprType = expr->getType();
   VariableType           *thisType = type;

   if (exprType->getBasicType() == VariableType::BT_ARRAY)
      exprType = exprType->getReturn()->getPointer();
   else
      exprType = exprType->getUnqualified();

   // Can cast a type to itself safely, of course.
   if (exprType == type) return;

   VariableType::BasicType exprBT = exprType->getBasicType();
   VariableType::BasicType thisBT = thisType->getBasicType();

   // Implicit cast between fundamentally different types.
   if (exprBT != thisBT)
   {
      // Can implicitly cast between arithmetic types.
      if (VariableType::is_bt_arithmetic(exprBT) &&
          VariableType::is_bt_arithmetic(thisBT))
         return;

      // Can implicitly cast between integer and pointer.
      if (VariableType::is_bt_integer(exprBT) &&
          thisBT == VariableType::BT_POINTER)
         return;

      if (exprBT == VariableType::BT_POINTER &&
          VariableType::is_bt_integer(thisBT))
         return;

      // Can implicitly cast from enum to integer.
      if (exprBT == VariableType::BT_ENUM &&
          VariableType::is_bt_integer(thisBT))
         return;

      // Can implicitly cast from block to array/struct.
      if (exprBT == VariableType::BT_BLOCK &&
          (thisBT == VariableType::BT_ARRAY ||
           thisBT == VariableType::BT_STRUCT))
         return;

      // Can always cast to void.
      if (thisBT == VariableType::BT_VOID)
         return;

      // Anything else must be explicit.
      BAD_CAST();
   }

   // Implicit pointer casts.
   // This is more restrictive than C's rules.
   if (exprBT == VariableType::BT_POINTER)
   {
      VariableType::Reference exprRetn = exprType->getReturn();
      VariableType::Reference thisRetn = thisType->getReturn();

      VariableType::BasicType exprRetnBT = exprRetn->getBasicType();
      VariableType::BasicType thisRetnBT = thisRetn->getBasicType();

      // Can cast if the reference type is exactly the same.
      if (exprRetn == thisRetn)
         return;

      // Casts that discard qualifiers must be explicit.
      if (!thisRetn->getQualifier(exprRetn->getQualifiers()))
      {
         BAD_CAST();
      }

      // Altering storage qualifiers must be explicit.
      if (exprRetn->getStoreType() != thisRetn->getStoreType() ||
          exprRetn->getStoreArea() != thisRetn->getStoreArea())
      {
         BAD_CAST();
      }

      // Cast to/from void* is acceptable.
      if (exprRetnBT == VariableType::BT_VOID ||
          thisRetnBT == VariableType::BT_VOID)
         return;

      // Any difference here is either added qualifiers, which is safe, or...
      if (exprRetn->getUnqualified() == thisRetn->getUnqualified())
         return;

      // ... different types, which (unlike C) is not allowed implicitly. This
      // also means that qualifiers can only be added to the first-referred
      // type. This correctly prevents implicit (T const **)T **. However, it
      // incorrectly prevents implicit (T const *const *)T * const*.
      BAD_CAST();
   }

   #undef BAD_CAST
}

//
// SourceExpression_ValueCast::canMakeObject
//
// Can make object if contained expression can, and source type is capable and
// destination type is capable.
//
// The latter two conditions are to be considered deficiencies of the engine.
//
bool SourceExpression_ValueCast::canMakeObject() const
{
   VariableType::Reference exprType = expr->getType()->getUnqualified();
   VariableType           *thisType = type;

   VariableType::BasicType exprBT = exprType->getBasicType();
   VariableType::BasicType thisBT = thisType->getBasicType();

   // Special case for casting an array to a pointer.
   if (exprBT == VariableType::BT_ARRAY &&
       thisBT == VariableType::BT_POINTER)
   {
      // Not the most efficient solution, no.
      return create_unary_reference(expr, context, position)->canMakeObject();
   }

   if (!expr->canMakeObject()) return false;

   switch (exprBT)
   {
   case VariableType::BT_ARRAY:
   case VariableType::BT_ASMFUNC:
   case VariableType::BT_STRUCT:
   case VariableType::BT_UNION:
   case VariableType::BT_VOID:
      return false;

   case VariableType::BT_BLOCK:
      return thisBT == VariableType::BT_ARRAY ||
             thisBT == VariableType::BT_BLOCK ||
             thisBT == VariableType::BT_STRUCT;

   case VariableType::BT_BOOLHARD:
   case VariableType::BT_BOOLSOFT:
   case VariableType::BT_CHAR:
   case VariableType::BT_ENUM:
   case VariableType::BT_FIXED:
   case VariableType::BT_FLOAT:
   case VariableType::BT_FUNCTION:
   case VariableType::BT_INT:
   case VariableType::BT_LABEL:
   case VariableType::BT_LFLOAT:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_LLFLOAT:
   case VariableType::BT_LLONG:
   case VariableType::BT_LONG:
   case VariableType::BT_NATIVE:
   case VariableType::BT_POINTER:
   case VariableType::BT_REAL:
   case VariableType::BT_SCHAR:
   case VariableType::BT_SCRIPT:
   case VariableType::BT_SHORT:
   case VariableType::BT_STRING:
   case VariableType::BT_UCHAR:
   case VariableType::BT_UINT:
   case VariableType::BT_ULLONG:
   case VariableType::BT_ULONG:
   case VariableType::BT_USHORT:
      break;
   }

   switch (thisBT)
   {
   case VariableType::BT_ARRAY:
   case VariableType::BT_ASMFUNC:
   case VariableType::BT_BLOCK:
   case VariableType::BT_STRUCT:
   case VariableType::BT_UNION:
   case VariableType::BT_VOID:
      return false;

   case VariableType::BT_BOOLHARD:
   case VariableType::BT_BOOLSOFT:
   case VariableType::BT_CHAR:
   case VariableType::BT_ENUM:
   case VariableType::BT_FIXED:
   case VariableType::BT_FLOAT:
   case VariableType::BT_FUNCTION:
   case VariableType::BT_INT:
   case VariableType::BT_LABEL:
   case VariableType::BT_LFLOAT:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_LLFLOAT:
   case VariableType::BT_LLONG:
   case VariableType::BT_LONG:
   case VariableType::BT_NATIVE:
   case VariableType::BT_POINTER:
   case VariableType::BT_REAL:
   case VariableType::BT_SCHAR:
   case VariableType::BT_SCRIPT:
   case VariableType::BT_SHORT:
   case VariableType::BT_STRING:
   case VariableType::BT_UCHAR:
   case VariableType::BT_UINT:
   case VariableType::BT_ULLONG:
   case VariableType::BT_ULONG:
   case VariableType::BT_USHORT:
      break;
   }

   return true;
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
      return create_unary_reference(expr, context, position)->makeObject();
   }

   return make_object_cast(expr->makeObject(), thisType, exprType, position);
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
      return create_unary_reference(expr, context, position)
      ->makeObjectsCast(objects, dst, type);
   }
   else
      expr->makeObjectsCast(objects, dst, type);
}

// EOF

