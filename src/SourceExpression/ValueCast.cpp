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
   SourceExpression_ValueCast(SourceExpression *expr, VariableType const *type,
                              bool implicit, SRCEXP_EXPR_ARGS);

   virtual bool canMakeObject() const;

   virtual VariableType const *getType() const;

   virtual ObjectExpression::Pointer makeObject() const;

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer expr;
   VariableType const *type;
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
SourceExpression_ValueCast::
SourceExpression_ValueCast(SourceExpression *_expr, VariableType const *_type,
                           bool implicit, SRCEXP_EXPR_PARM)
                           : Super(SRCEXP_EXPR_PASS),
                             expr(_expr), type(_type)
{
   #define BAD_CAST()                                           \
   if (true)                                                    \
   {                                                            \
      exceptString  = "invalid implicit cast (";                \
      exceptString += make_string(exprType->vt);                \
      exceptString += " to ";                                   \
      exceptString += make_string(type->vt);                    \
      exceptString += ")";                                      \
      throw SourceException(exceptString, position, getName()); \
   }                                                            \
   else (void)0

   // Explicit casts bypass all tests. (For now.)
   if (!implicit) return;

   std::string exceptString;

   VariableType const *exprType = expr->getType();

   // Can cast a type to itself safely, of course.
   if (exprType == type) return;

   // Can cast T const to/from T safely.
   if (exprType->constType == type || type->constType == exprType)
      return;

   // Implicit cast between fundamentally different types.
   if (exprType->vt != type->vt)
   {
      // Any cast to/from VT_ASMFUNC must be explicit.
      if (exprType->vt == VariableType::VT_ASMFUNC ||
          type->vt     == VariableType::VT_ASMFUNC)
         BAD_CAST();

      // Any cast to/from VT_FUNCTION must be explicit.
      if (exprType->vt == VariableType::VT_FUNCTION ||
          type->vt     == VariableType::VT_FUNCTION)
         BAD_CAST();

      // Any cast to/from VT_LABEL must be explicit.
      if (exprType->vt == VariableType::VT_LABEL ||
          type->vt     == VariableType::VT_LABEL)
         BAD_CAST();

      // Any cast to/from VT_LINESPEC must be explicit.
      if (exprType->vt == VariableType::VT_LINESPEC ||
          type->vt     == VariableType::VT_LINESPEC)
         BAD_CAST();

      // Any cast to/from VT_NATIVE must be explicit.
      if (exprType->vt == VariableType::VT_NATIVE ||
          type->vt     == VariableType::VT_NATIVE)
         BAD_CAST();

      // Any cast to/from VT_SCRIPT must be explicit.
      if (exprType->vt == VariableType::VT_SCRIPT ||
          type->vt     == VariableType::VT_SCRIPT)
         BAD_CAST();

      // Any cast to/from VT_STRING must be explicit.
      if (exprType->vt == VariableType::VT_STRING ||
          type->vt     == VariableType::VT_STRING)
         BAD_CAST();

      return;
   }

   // Implicit pointer casts.
   // This is more restrictive than C's rules.
   if (exprType->vt == VariableType::VT_POINTER)
   {
      // Can cast if the refType is the same.
      if (exprType->refType == type->refType)
         return;

      // Casts that discard const must be explicit.
      if (!exprType->refType->constType && type->refType->constType)
         BAD_CAST();

      // Cast to/from void* is acceptable.
      if (exprType->refType->vt == VariableType::VT_VOID ||
          type->refType->vt     == VariableType::VT_VOID)
         return;

      // Cast to T const * from T * is safe.
      if (exprType->refType->constType == type->refType)
         return;

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
   if (!expr->canMakeObject()) return false;

   VariableType const *exprType = expr->getType();

   switch (type->vt)
   {
   case VariableType::VT_ARRAY:
   case VariableType::VT_ASMFUNC:
   case VariableType::VT_BLOCK:
   case VariableType::VT_STRUCT:
   case VariableType::VT_UNION:
   case VariableType::VT_VOID:
      return false;

   case VariableType::VT_BOOLHARD:
   case VariableType::VT_BOOLSOFT:
   case VariableType::VT_CHAR:
   case VariableType::VT_ENUM:
   case VariableType::VT_FUNCTION:
   case VariableType::VT_INT:
   case VariableType::VT_LABEL:
   case VariableType::VT_LINESPEC:
   case VariableType::VT_NATIVE:
   case VariableType::VT_POINTER:
   case VariableType::VT_REAL:
   case VariableType::VT_SCRIPT:
   case VariableType::VT_STRING:
      break;
   }

   switch (exprType->vt)
   {
   case VariableType::VT_ARRAY:
   case VariableType::VT_ASMFUNC:
   case VariableType::VT_BLOCK:
   case VariableType::VT_STRUCT:
   case VariableType::VT_UNION:
   case VariableType::VT_VOID:
      return false;

   case VariableType::VT_BOOLHARD:
   case VariableType::VT_BOOLSOFT:
   case VariableType::VT_CHAR:
   case VariableType::VT_ENUM:
   case VariableType::VT_FUNCTION:
   case VariableType::VT_INT:
   case VariableType::VT_LABEL:
   case VariableType::VT_LINESPEC:
   case VariableType::VT_NATIVE:
   case VariableType::VT_POINTER:
   case VariableType::VT_REAL:
   case VariableType::VT_SCRIPT:
   case VariableType::VT_STRING:
      break;
   }

   return true;
}

//
// SourceExpression_ValueCast::getType
//
VariableType const *SourceExpression_ValueCast::getType() const
{
   return type;
}

//
// SourceExpression_ValueCast::makeObject
//
ObjectExpression::Pointer SourceExpression_ValueCast::makeObject() const
{
   ObjectExpression::Pointer obj = expr->makeObject();

   VariableType const *exprType = expr->getType();

   switch (type->vt)
   {
   case VariableType::VT_ARRAY:
   case VariableType::VT_ASMFUNC:
   case VariableType::VT_BLOCK:
   case VariableType::VT_STRUCT:
   case VariableType::VT_UNION:
   case VariableType::VT_VOID:
      throw SourceException("invalid VT from", position, getName());

   case VariableType::VT_BOOLHARD:
   case VariableType::VT_CHAR:
   case VariableType::VT_ENUM:
   case VariableType::VT_FUNCTION:
   case VariableType::VT_INT:
   case VariableType::VT_LABEL:
   case VariableType::VT_LINESPEC:
   case VariableType::VT_NATIVE:
   case VariableType::VT_POINTER:
   case VariableType::VT_SCRIPT:
   case VariableType::VT_STRING:
      switch (exprType->vt)
      {
      case VariableType::VT_ARRAY:
      case VariableType::VT_ASMFUNC:
      case VariableType::VT_BLOCK:
      case VariableType::VT_STRUCT:
      case VariableType::VT_UNION:
      case VariableType::VT_VOID:
         throw SourceException("invalid VT to", position, getName());

      case VariableType::VT_BOOLHARD:
         obj = ObjectExpression::create_unary_lognot(obj, position);
         obj = ObjectExpression::create_unary_lognot(obj, position);
         break;

      case VariableType::VT_BOOLSOFT:
      case VariableType::VT_CHAR:
      case VariableType::VT_ENUM:
      case VariableType::VT_FUNCTION:
      case VariableType::VT_INT:
      case VariableType::VT_LABEL:
      case VariableType::VT_LINESPEC:
      case VariableType::VT_NATIVE:
      case VariableType::VT_POINTER:
      case VariableType::VT_SCRIPT:
      case VariableType::VT_STRING:
         break;

      case VariableType::VT_REAL:
         obj = ObjectExpression::create_cast_int_to_float(obj, position);
         break;
      }
      break;

   case VariableType::VT_BOOLSOFT:
      switch (exprType->vt)
      {
      case VariableType::VT_ARRAY:
      case VariableType::VT_ASMFUNC:
      case VariableType::VT_BLOCK:
      case VariableType::VT_STRUCT:
      case VariableType::VT_UNION:
      case VariableType::VT_VOID:
         throw SourceException("invalid VT to", position, getName());

      case VariableType::VT_BOOLHARD:
      case VariableType::VT_CHAR:
      case VariableType::VT_ENUM:
      case VariableType::VT_FUNCTION:
      case VariableType::VT_INT:
      case VariableType::VT_LABEL:
      case VariableType::VT_LINESPEC:
      case VariableType::VT_NATIVE:
      case VariableType::VT_POINTER:
      case VariableType::VT_SCRIPT:
      case VariableType::VT_STRING:
         obj = ObjectExpression::create_unary_lognot(obj, position);
         obj = ObjectExpression::create_unary_lognot(obj, position);
         break;

      case VariableType::VT_BOOLSOFT:
         break;

      case VariableType::VT_REAL:
         obj = ObjectExpression::create_unary_lognot(obj, position);
         obj = ObjectExpression::create_unary_lognot(obj, position);
         obj = ObjectExpression::create_cast_int_to_float(obj, position);
         break;
      }
      break;

   case VariableType::VT_REAL:
      switch (exprType->vt)
      {
      case VariableType::VT_ARRAY:
      case VariableType::VT_ASMFUNC:
      case VariableType::VT_BLOCK:
      case VariableType::VT_STRUCT:
      case VariableType::VT_UNION:
      case VariableType::VT_VOID:
         throw SourceException("invalid VT to", position, getName());

      case VariableType::VT_BOOLHARD:
         obj = ObjectExpression::create_cast_float_to_int(obj, position);
         obj = ObjectExpression::create_unary_lognot(obj, position);
         obj = ObjectExpression::create_unary_lognot(obj, position);
         break;

      case VariableType::VT_BOOLSOFT:
      case VariableType::VT_REAL:
         break;

      case VariableType::VT_CHAR:
      case VariableType::VT_ENUM:
      case VariableType::VT_FUNCTION:
      case VariableType::VT_INT:
      case VariableType::VT_LABEL:
      case VariableType::VT_LINESPEC:
      case VariableType::VT_NATIVE:
      case VariableType::VT_POINTER:
      case VariableType::VT_SCRIPT:
      case VariableType::VT_STRING:
         obj = ObjectExpression::create_cast_float_to_int(obj, position);
         break;
      }
      break;
   }

   return obj;
}

//
// SourceExpression_ValueCast::virtual_makeObjects
//
void SourceExpression_ValueCast::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   // Special case for casting an array to a pointer.
   if (expr->getType()->vt == VariableType::VT_ARRAY &&
      type->vt == VariableType::VT_POINTER)
   {
      expr->makeObjectsAddress(objects, dst);
   }
   else
      expr->makeObjectsCast(objects, dst, type);
}

// EOF

