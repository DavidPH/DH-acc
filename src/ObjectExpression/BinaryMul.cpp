//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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
// ObjectExpression handling of "operator *".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_BinaryMul
//
class ObjectExpression_BinaryMul : public ObjectExpression_Binary
{
   CounterPreambleNoClone(ObjectExpression_BinaryMul, ObjectExpression_Binary);

public:
   ObjectExpression_BinaryMul(OBJEXP_EXPRBIN_PARM) : Super(OBJEXP_EXPRBIN_PASS) {}
   ObjectExpression_BinaryMul(ObjectLoad &arc) : Super(arc) {}

   bigreal resolveFLT() const {return exprL->resolveFLT() * exprR->resolveFLT();}
   bigreal resolveFIX() const {return exprL->resolveFIX() * exprR->resolveFIX();}
   bigsint resolveINT() const {return exprL->resolveINT() * exprR->resolveINT();}
   biguint resolveUNS() const {return exprL->resolveUNS() * exprR->resolveUNS();}

protected:
   //
   // save
   //
   virtual ObjectSave &save(ObjectSave &arc) const
   {
      return Super::save(arc << OT_BINARY_MUL);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_binary_mul
//
ObjectExpression::Reference ObjectExpression::create_binary_mul(OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryMul(exprL, exprR, pos));
}

//
// ObjectExpression::LoadBinaryMul
//
auto ObjectExpression::LoadBinaryMul(ObjectLoad &arc) -> Reference
{
   return static_cast<Reference>(new ObjectExpression_BinaryMul(arc));
}

// EOF

