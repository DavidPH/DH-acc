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
// ObjectExpression handling of "operator &&".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_BranchIOr
//
class ObjectExpression_BranchIOr : public ObjectExpression_Binary
{
   CounterPreambleNoClone(ObjectExpression_BranchIOr, ObjectExpression_Binary);

public:
   ObjectExpression_BranchIOr(OBJEXP_EXPRBIN_PARM) : Super(OBJEXP_EXPRBIN_PASS) {}
   ObjectExpression_BranchIOr(ObjectLoad &arc) : Super(arc) {}

   virtual ExpressionType getType() const {return ET_INT;}

   //
   // resolveINT
   //
   virtual bigsint resolveINT() const
   {
      switch(exprL->getType())
      {
      case ET_FIX_HH:
      case ET_FIX_H:
      case ET_FIX:
      case ET_FIX_L:
      case ET_FIX_LL:
         return exprL->resolveFIX() || exprR->resolveFIX();

      case ET_FLT_HH:
      case ET_FLT_H:
      case ET_FLT:
      case ET_FLT_L:
      case ET_FLT_LL:
         return exprL->resolveFLT() || exprR->resolveFLT();

      case ET_INT_HH:
      case ET_INT_H:
      case ET_INT:
      case ET_INT_L:
      case ET_INT_LL:
         return exprL->resolveINT() || exprR->resolveINT();

      case ET_UNS_HH:
      case ET_UNS_H:
      case ET_UNS:
      case ET_UNS_L:
      case ET_UNS_LL:
         return exprL->resolveUNS() || exprR->resolveUNS();

      case ET_OCS: break;

      case ET_ARR: break;
      case ET_MAP: break;
      }

      return Super::resolveINT();
   }

protected:
   //
   // save
   //
   virtual ObjectSave &save(ObjectSave &arc) const
   {
      return Super::save(arc << OT_BRANCH_IOR);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_branch_ior
//
ObjectExpression::Reference ObjectExpression::create_branch_ior(OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BranchIOr(exprL, exprR, pos));
}

//
// ObjectExpression::LoadBranchIOr
//
auto ObjectExpression::LoadBranchIOr(ObjectLoad &arc) -> Reference
{
   return static_cast<Reference>(new ObjectExpression_BranchIOr(arc));
}

// EOF

