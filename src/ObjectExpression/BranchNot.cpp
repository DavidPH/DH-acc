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
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_BranchNot,
                                   ObjectExpression_Unary);

public:
   //
   // ::ObjectExpression_BranchNot
   //
   ObjectExpression_BranchNot(OBJEXP_EXPRUNA_PARM) : Super(OBJEXP_EXPRUNA_PASS)
   {
   }

   //
   // ::ObjectExpression_BranchNot
   //
   ObjectExpression_BranchNot(std::istream *in)
   : Super(in)
   {
   }

   //
   // ::getType
   //
   virtual ExpressionType getType() const
   {
      return ET_INT;
   }

   //
   // ::resolveINT
   //
   virtual bigsint resolveINT() const
   {
      switch(expr->getType())
      {
      case ET_FIX: return !expr->resolveFIX();
      case ET_FLT: return !expr->resolveFLT();
      case ET_INT: return !expr->resolveINT();
      case ET_UNS: return !expr->resolveUNS();
      case ET_OCS: break;

      case ET_ARR: break;
      case ET_MAP: break;
      }

      return Super::resolveINT();
   }

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream *out) const
   {
      write_object(out, OT_BRANCH_NOT);

      Super::writeObject(out);
   }

private:
   //
   // ::writeACSPLong
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
// ObjectExpression::create_branch_not
//
ObjectExpression::Reference ObjectExpression::create_branch_not(std::istream *in)
{
   return static_cast<Reference>(new ObjectExpression_BranchNot(in));
}

// EOF

