//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 David Hill
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
// ObjectExpression handling of "operator !".
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_BranchNot
//
class ObjectExpression_BranchNot : public ObjectExpression_Unary
{
   CounterPreambleNoClone(ObjectExpression_BranchNot, ObjectExpression_Unary);

public:
   ObjectExpression_BranchNot(OBJEXP_EXPRUNA_PARM) : Super(OBJEXP_EXPRUNA_PASS) {}
   ObjectExpression_BranchNot(ObjectArchive &arc) : Super(arc) {}

   virtual ExpressionType getType() const {return ET_INT;}

   //
   // resolveINT
   //
   virtual bigsint resolveINT() const
   {
      switch(expr->getType())
      {
      case ET_FIX_HH:
      case ET_FIX_H:
      case ET_FIX:
      case ET_FIX_L:
      case ET_FIX_LL:
         return !expr->resolveFIX();

      case ET_FLT_HH:
      case ET_FLT_H:
      case ET_FLT:
      case ET_FLT_L:
      case ET_FLT_LL:
         return !expr->resolveFLT();

      case ET_INT_HH:
      case ET_INT_H:
      case ET_INT:
      case ET_INT_L:
      case ET_INT_LL:
         return !expr->resolveINT();

      case ET_UNS_HH:
      case ET_UNS_H:
      case ET_UNS:
      case ET_UNS_L:
      case ET_UNS_LL:
         return !expr->resolveUNS();

      case ET_OCS: break;

      case ET_ARR: break;
      case ET_MAP: break;
      }

      return Super::resolveINT();
   }

protected:
   //
   // archive
   //
   virtual ObjectArchive &archive(ObjectArchive &arc)
   {
      return Super::archive(arc << OT_BRANCH_NOT);
   }

private:
   //
   // writeACSPLong
   //
   virtual void writeACSPLong(std::ostream *out) const
   {
      BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_LNOT);
      expr->writeACSP(out);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_branch_not
//
ObjectExpression::Reference ObjectExpression::create_branch_not(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BranchNot(expr, pos));
}

//
// ObjectExpression::CreateBranchNot
//
ObjectExpression::Reference ObjectExpression::CreateBranchNot(ObjectArchive &arc)
{
   return static_cast<Reference>(new ObjectExpression_BranchNot(arc));
}

// EOF

