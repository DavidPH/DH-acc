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
// ObjectExpression_BranchAnd
//
class ObjectExpression_BranchAnd : public ObjectExpression_Binary
{
   CounterPreambleNoClone(ObjectExpression_BranchAnd, ObjectExpression_Binary);

public:
   ObjectExpression_BranchAnd(OBJEXP_EXPRBIN_PARM) : Super(OBJEXP_EXPRBIN_PASS) {}
   ObjectExpression_BranchAnd(ObjectArchive &arc) : Super(arc) {}

   virtual ExpressionType getType() const {return ET_INT;}

   //
   // resolveINT
   //
   virtual bigsint resolveINT() const
   {
      switch(exprL->getType())
      {
      case ET_FIX: return exprL->resolveFIX() && exprR->resolveFIX();
      case ET_FLT: return exprL->resolveFLT() && exprR->resolveFLT();
      case ET_INT: return exprL->resolveINT() && exprR->resolveINT();
      case ET_UNS: return exprL->resolveUNS() && exprR->resolveUNS();
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
      return Super::archive(arc << OT_BRANCH_AND);
   }

private:
   //
   // writeACSPLong
   //
   virtual void writeACSPLong(std::ostream *out) const
   {
      BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_LAND);

      exprL->writeACSP(out);
      exprR->writeACSP(out);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_branch_and
//
ObjectExpression::Reference ObjectExpression::create_branch_and(OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BranchAnd(exprL, exprR, pos));
}

//
// ObjectExpression::CreateBranchAnd
//
ObjectExpression::Reference ObjectExpression::CreateBranchAnd(ObjectArchive &arc)
{
   return static_cast<Reference>(new ObjectExpression_BranchAnd(arc));
}

// EOF

