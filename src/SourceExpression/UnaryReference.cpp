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
      return expr->canMakeObjectAddress();
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
      return expr->makeObjectAddress();
   }

private:
   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      expr->makeObjectsAddress(objects, dst);
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
   return new SourceExpression_UnaryReference(expr, context, position);
}

// EOF

