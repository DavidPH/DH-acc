//-----------------------------------------------------------------------------
//
// Copyright(C) 2013 David Hill
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
// Handles dividing up of multi-byte binary representation types.
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"

#include "../ObjectArchive.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_ValuePart
//
class ObjectExpression_ValuePart : public ObjectExpression_Unary
{
   CounterPreambleNoClone(ObjectExpression_ValuePart, ObjectExpression_Unary);

public:
   ObjectExpression_ValuePart(biguint _part, OBJEXP_EXPRUNA_PARM)
    : Super(OBJEXP_EXPRUNA_PASS), part(_part) {}
   ObjectExpression_ValuePart(ObjectArchive &arc) : Super(arc) {arc << part;}

   virtual bool canResolve() const {return false;}

   virtual ExpressionType getType() const {return ET_UNS;}

   // Technically, resolving should be forbidden. Technically.
   virtual biguint resolveUNS() const {return resolveBinary(0);}

   virtual biguint resolveBinary(biguint) const {return expr->resolveBinary(part);}

protected:
   //
   // archive
   //
   virtual ObjectArchive &archive(ObjectArchive &arc)
   {
      return Super::archive(arc << OT_VALUE_PART) << part;
   }

private:
   biguint part;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::CreateValuePart
//
ObjectExpression::Reference ObjectExpression::CreateValuePart(
   biguint part, ObjectExpression *expr, OBJEXP_EXPR_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_ValuePart(part, expr, pos));
}

//
// ObjectExpression::CreateValuePart
//
ObjectExpression::Reference ObjectExpression::CreateValuePart(ObjectArchive &arc)
{
   return static_cast<Reference>(new ObjectExpression_ValuePart(arc));
}

// EOF

