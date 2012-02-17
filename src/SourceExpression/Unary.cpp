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
// Base class for source-level unary expression handling.
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"

#include "../ObjectVector.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression_Unary::SourceExpression_Unary
//
SourceExpression_Unary::SourceExpression_Unary
(SRCEXP_EXPRUNA_PARM)
 : Super(SRCEXP_EXPR_PASS), expr(_expr), make(false)
{
}

//
// SourceExpression_Unary::SourceExpression_Unary
//
SourceExpression_Unary::SourceExpression_Unary
(VariableType *cast, SRCEXP_EXPRUNA_PARM)
 : Super(SRCEXP_EXPR_PASS), expr(_expr), make(true)
{
   if (cast)
      expr = create_value_cast_implicit(expr, cast, context, position);
}

//
// SourceExpression_Unary::getType
//
VariableType::Reference SourceExpression_Unary::getType() const
{
   return expr->getType();
}

//
// SourceExpression_Unary::recurse_makeObjects
//
void SourceExpression_Unary::recurse_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   // Special case, child handles expression.
   if (!make) return;

   VariableData::Pointer src =
      VariableData::create_stack(getType()->getSize(position));

   make_objects_memcpy_prep(objects, dst, src, position);
   expr->makeObjects(objects, src);

   objects->setPosition(position);
}

// EOF

