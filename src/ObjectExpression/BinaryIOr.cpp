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
// ObjectExpression handling of "operator |".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_BinaryIOr
//
class ObjectExpression_BinaryIOr : public ObjectExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_BinaryIOr,
                                   ObjectExpression_Binary);

public:
   //
   // ::ObjectExpression_BinaryIOr
   //
   ObjectExpression_BinaryIOr(OBJEXP_EXPRBIN_PARM) : Super(OBJEXP_EXPRBIN_PASS)
   {
   }

   //
   // ::ObjectExpression_BinaryIOr
   //
   ObjectExpression_BinaryIOr(std::istream *in) : Super(in)
   {
   }

   bigsint resolveINT() const {return exprL->resolveINT() | exprR->resolveINT();}
   biguint resolveUNS() const {return exprL->resolveUNS() | exprR->resolveUNS();}

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream *out) const
   {
      write_object(out, OT_BINARY_IOR);

      Super::writeObject(out);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_binary_ior
//
ObjectExpression::Reference ObjectExpression::create_binary_ior(OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryIOr(exprL, exprR, pos));
}

//
// ObjectExpression::create_binary_ior
//
ObjectExpression::Reference ObjectExpression::create_binary_ior(std::istream *in)
{
   return static_cast<Reference>(new ObjectExpression_BinaryIOr(in));
}

// EOF

