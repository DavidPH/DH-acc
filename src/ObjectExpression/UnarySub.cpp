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
// ObjectExpression handling of "operator -".
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_UnarySub
//
class ObjectExpression_UnarySub : public ObjectExpression_Unary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_UnarySub,
                                   ObjectExpression_Unary);

public:
   //
   // ::ObjectExpression_UnarySub
   //
   ObjectExpression_UnarySub(OBJEXP_EXPRUNA_PARM) : Super(OBJEXP_EXPRUNA_PASS)
   {
   }

   //
   // ::ObjectExpression_UnarySub
   //
   ObjectExpression_UnarySub(std::istream *in) : Super(in)
   {
   }

   bigreal resolveFIX() const {return -expr->resolveFIX();}
   bigreal resolveFLT() const {return -expr->resolveFLT();}
   bigsint resolveINT() const {return -expr->resolveINT();}
   biguint resolveUNS() const {return -expr->resolveUNS();}

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream *out) const
   {
      write_object(out, OT_UNARY_SUB);

      Super::writeObject(out);
   }

private:
   //
   // ::writeACSPLong
   //
   virtual void writeACSPLong(std::ostream *out) const
   {
      BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_NEGATE);

      expr->writeACSP(out);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_unary_sub
//
ObjectExpression::Reference ObjectExpression::create_unary_sub(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_UnarySub(expr, pos));
}

//
// ObjectExpression::create_unary_sub
//
ObjectExpression::Reference ObjectExpression::create_unary_sub(std::istream *in)
{
   return static_cast<Reference>(new ObjectExpression_UnarySub(in));
}

// EOF

