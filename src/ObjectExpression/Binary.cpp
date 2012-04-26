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
// Base class for object-level binary expression handling.
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
ObjectExpression_Binary::ObjectExpression_Binary
(ObjectExpression *_exprL, ObjectExpression *_exprR, SourcePosition const &_pos)
 : Super(_pos), exprL(_exprL), exprR(_exprR)
{
   if (exprL->getType() != exprR->getType())
      ERROR_N(position, "type mismatch");
}

//
// ObjectExpression_Binary::ObjectExpression_Binary
//
ObjectExpression_Binary::ObjectExpression_Binary(std::istream * in) : Super(in)
{
	read_object(in, &exprL);
	read_object(in, &exprR);
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

