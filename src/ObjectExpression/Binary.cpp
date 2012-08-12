//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2012 David Hill
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
// ObjectExpression handling of binary expressions.
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../SourceException.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression_Binary::ObjectExpression_Binary
//
ObjectExpression_Binary::ObjectExpression_Binary(OBJEXP_EXPRBIN_PARM)
 : Super(OBJEXP_EXPR_PASS), exprL(_exprL), exprR(_exprR)
{
   ExpressionType typeL = exprL->getType(), typeR = exprR->getType();

   // HACK! Allow type mismatch for INT vs UNS.
   if((typeL == ET_INT && typeR == ET_UNS) || (typeL == ET_UNS && typeR == ET_INT))
      return;

   if(typeL != typeR)
      Error_NP("type mismatch: %s %s", make_string(typeL).c_str(), make_string(typeR).c_str());
}

//
// ObjectExpression_Binary::ObjectExpression_Binary
//
ObjectExpression_Binary::ObjectExpression_Binary(std::istream *in)
 : Super(in), exprL(create(in)), exprR(create(in))
{
}

//
// ObjectExpression_Binary::canResolve
//
bool ObjectExpression_Binary::canResolve() const
{
   return exprL->canResolve() && exprR->canResolve();
}

//
// ObjectExpression_Binary::getType
//
ObjectExpression::ExpressionType ObjectExpression_Binary::getType() const
{
   return exprL->getType();
}

//
// ObjectExpression_Binary::writeObject
//
void ObjectExpression_Binary::writeObject(std::ostream *out) const
{
   Super::writeObject(out);

   write_object(out, &exprL);
   write_object(out, &exprR);
}

// EOF

