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
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_BranchIf,
                                   ObjectExpression_Binary);

public:
   //
   // ::ObjectExpression_BranchIf
   //
   ObjectExpression_BranchIf(OBJEXP_EXPRTRI_PARM)
    : Super(OBJEXP_EXPRBIN_PASS), exprC(_exprC)
   {
   }

   //
   // ::ObjectExpression_BranchIf
   //
   ObjectExpression_BranchIf(std::istream *in) : Super(in), exprC(create(in))
   {
   }

   //
   // ::canResolve
   //
   virtual bool canResolve() const
   {
      return exprC->canResolve() && Super::canResolve();
   }

   //
   // ::resolveFloat
   //
   virtual bigreal resolveFloat() const
   {
      return (exprC->resolveInt() ? exprL : exprR)->resolveFloat();
   }

   //
   // ::resolveOCode
   //
   virtual ObjectCodeSet resolveOCode() const
   {
      return (exprC->resolveInt() ? exprL : exprR)->resolveOCode();
   }

   //
   // ::resolveInt
   //
   virtual bigsint resolveInt() const
   {
      return (exprC->resolveInt() ? exprL : exprR)->resolveInt();
   }

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream *out) const
   {
      write_object(out, OT_BRANCH_IF);

      Super::writeObject(out);

      write_object(out, &exprC);
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
ObjectExpression::Reference ObjectExpression::create_branch_if(
   OBJEXP_EXPRTRI_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BranchIf(
      exprC, exprL, exprR, pos));
}

//
// ObjectExpression::create_branch_if
//
ObjectExpression::Reference ObjectExpression::create_branch_if(std::istream *in)
{
   return static_cast<Reference>(new ObjectExpression_BranchIf(in));
}

// EOF

