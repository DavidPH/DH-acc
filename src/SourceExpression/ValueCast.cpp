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
                              SRCEXP_EXPR_ARGS);

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
// SourceExpression::create_value_cast
//
SRCEXP_EXPRVAL_DEFN(et, cast)
{
   return new SourceExpression_ValueCast(expr, type, context, position);
}

//
// SourceExpression_ValueCast::SourceExpression_ValueCast
//
SourceExpression_ValueCast::
SourceExpression_ValueCast(SourceExpression *_expr, VariableType const *_type,
                           SRCEXP_EXPR_PARM)
                           : Super(SRCEXP_EXPR_PASS),
                             expr(_expr), type(_type)
{
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

