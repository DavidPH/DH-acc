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
   SourceExpression_UnaryReference(SRCEXP_EXPRUNA_ARGS);

   //
   // ::canMakeObject
   //
   virtual bool canMakeObject() const
   {
      return expr->canMakeObjectAddress();
   }

   virtual VariableType const *getType() const;

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      return expr->makeObjectAddress();
   }

private:
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   VariableType const *type;
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

//
// SourceExpression_UnaryReference::SourceExpression_UnaryReference
//
SourceExpression_UnaryReference::
SourceExpression_UnaryReference
(SRCEXP_EXPRUNA_PARM)
 : Super(SRCEXP_EXPRUNA_PASS),
   type(VariableType::get_pointer(expr->getType()))
{
}

//
// SourceExpression_UnaryReference::getType
//
VariableType const * SourceExpression_UnaryReference::getType() const
{
   return type;
}

//
// SourceExpression_UnaryReference::virtual_makeObjects
//
void SourceExpression_UnaryReference::
virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   expr->makeObjectsAddress(objects, dst);
}

// EOF

