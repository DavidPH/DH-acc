//-----------------------------------------------------------------------------
//
// Copyright(C) 2012-2013 David Hill
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
// ObjectExpression handling of "operator >>".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_BinaryRSh
//
class ObjectExpression_BinaryRSh : public ObjectExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_BinaryRSh,
                                   ObjectExpression_Binary);

public:
   ObjectExpression_BinaryRSh(OBJEXP_EXPRBIN_PARM) : Super(OBJEXP_EXPRBIN_PASS) {}
   ObjectExpression_BinaryRSh(ObjectArchive &arc) : Super(arc) {}

   bigsint resolveINT() const {return exprL->resolveINT() >> exprR->resolveINT();}
   biguint resolveUNS() const {return exprL->resolveUNS() >> exprR->resolveUNS();}

protected:
   //
   // archive
   //
   virtual ObjectArchive &archive(ObjectArchive &arc)
   {
      return Super::archive(arc << OT_BINARY_RSH);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_binary_rsh
//
ObjectExpression::Reference ObjectExpression::create_binary_rsh(OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryRSh(exprL, exprR, pos));
}

//
// ObjectExpression::CreateBinaryRSh
//
ObjectExpression::Reference ObjectExpression::CreateBinaryRSh(ObjectArchive &arc)
{
   return static_cast<Reference>(new ObjectExpression_BinaryRSh(arc));
}

// EOF

