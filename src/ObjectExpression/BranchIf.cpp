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
// ObjectExpression handling of "operator ?:".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"
#include "../ObjectCode.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_BranchIf
//
class ObjectExpression_BranchIf : public ObjectExpression_Binary
{
   CounterPreambleNoClone(ObjectExpression_BranchIf, ObjectExpression_Binary);

public:
   ObjectExpression_BranchIf(OBJEXP_EXPRTRI_PARM) : Super(OBJEXP_EXPRBIN_PASS),
      exprC(_exprC) {}
   ObjectExpression_BranchIf(ObjectLoad &arc) : Super(arc), exprC(LoadExpr(arc)) {}

   //
   // canResolve
   //
   virtual bool canResolve() const
   {
      return exprC->canResolve() && Super::canResolve();
   }

   bigreal resolveFIX() const {return (exprC->resolveINT() ? exprL : exprR)->resolveFIX();}
   bigreal resolveFLT() const {return (exprC->resolveINT() ? exprL : exprR)->resolveFLT();}
   bigsint resolveINT() const {return (exprC->resolveINT() ? exprL : exprR)->resolveINT();}
   biguint resolveUNS() const {return (exprC->resolveINT() ? exprL : exprR)->resolveUNS();}
   ObjectCodeSet resolveOCS() const {return (exprC->resolveINT() ? exprL : exprR)->resolveOCS();}

protected:
   //
   // save
   //
   virtual ObjectSave &save(ObjectSave &arc) const
   {
      return Super::save(arc << OT_BRANCH_IF) << exprC;
   }

private:
   ObjectExpression::Reference exprC;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_branch_if
//
ObjectExpression::Reference ObjectExpression::create_branch_if(OBJEXP_EXPRTRI_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BranchIf(exprC, exprL, exprR, pos));
}

//
// ObjectExpression::LoadBranchIf
//
auto ObjectExpression::LoadBranchIf(ObjectLoad &arc) -> Reference
{
   return static_cast<Reference>(new ObjectExpression_BranchIf(arc));
}

// EOF

