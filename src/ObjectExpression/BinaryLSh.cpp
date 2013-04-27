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
// ObjectExpression handling of "operator <<".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_BinaryLSh
//
class ObjectExpression_BinaryLSh : public ObjectExpression_Binary
{
   CounterPreambleNoClone(ObjectExpression_BinaryLSh, ObjectExpression_Binary);

public:
   ObjectExpression_BinaryLSh(OBJEXP_EXPRBIN_PARM) : Super(OBJEXP_EXPRBIN_PASS) {}
   ObjectExpression_BinaryLSh(ObjectLoad &arc) : Super(arc) {}

   bigsint resolveINT() const {return exprL->resolveINT() << exprR->resolveINT();}
   biguint resolveUNS() const {return exprL->resolveUNS() << exprR->resolveUNS();}

protected:
   //
   // save
   //
   virtual ObjectSave &save(ObjectSave &arc) const
   {
      return Super::save(arc << OT_BINARY_LSH);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_binary_lsh
//
ObjectExpression::Reference ObjectExpression::create_binary_lsh(OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryLSh(exprL, exprR, pos));
}

//
// ObjectExpression::LoadBinaryLSh
//
auto ObjectExpression::LoadBinaryLSh(ObjectLoad &arc) -> Reference
{
   return static_cast<Reference>(new ObjectExpression_BinaryLSh(arc));
}

// EOF

