//-----------------------------------------------------------------------------
//
// Copyright(C) 2011 David Hill
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

   virtual VariableType const *getType() const;

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
// SourceExpression_ValueCast::getType
//
VariableType const *SourceExpression_ValueCast::getType() const
{
   return type;
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

