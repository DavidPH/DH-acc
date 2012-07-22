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
// ObjectExpression handling of "operator &".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_BinaryAnd
//
class ObjectExpression_BinaryAnd : public ObjectExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_BinaryAnd,
                                   ObjectExpression_Binary);

public:
   //
   // ::ObjectExpression_BinaryAnd
   //
   ObjectExpression_BinaryAnd(OBJEXP_EXPRBIN_PARM) : Super(OBJEXP_EXPRBIN_PASS)
   {
   }

   //
   // ::ObjectExpression_BinaryAnd
   //
   ObjectExpression_BinaryAnd(std::istream *in) : Super(in)
   {
   }

   bigsint resolveINT() const {return exprL->resolveINT() & exprR->resolveINT();}
   biguint resolveUNS() const {return exprL->resolveUNS() & exprR->resolveUNS();}

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream *out) const
   {
      write_object(out, OT_BINARY_AND);

      Super::writeObject(out);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_binary_and
//
ObjectExpression::Reference ObjectExpression::create_binary_and(OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryAnd(exprL, exprR, pos));
}

//
// ObjectExpression::create_binary_and
//
ObjectExpression::Reference ObjectExpression::create_binary_and(std::istream *in)
{
   return static_cast<Reference>(new ObjectExpression_BinaryAnd(in));
}

// EOF

