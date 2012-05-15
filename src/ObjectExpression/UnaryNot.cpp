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
// ObjectExpression handling of "operator ~".
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_UnaryNot
//
class ObjectExpression_UnaryNot : public ObjectExpression_Unary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_UnaryNot,
                                   ObjectExpression_Unary);

public:
   //
   // ::ObjectExpression_UnaryNot
   //
   ObjectExpression_UnaryNot(OBJEXP_EXPRUNA_PARM) : Super(OBJEXP_EXPRUNA_PASS)
   {
   }

   //
   // ::ObjectExpression_UnaryNot
   //
   ObjectExpression_UnaryNot(std::istream *in) : Super(in)
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
   // ::resolveInt
   //
   virtual bigsint resolveInt() const
   {
      return ~expr->resolveInt();
   }

   //
   // ::writeACSPLong
   //
   virtual void writeACSPLong(std::ostream *out) const
   {
      BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_BNOT);

      expr->writeACSP(out);
   }

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream *out) const
   {
      write_object(out, OT_UNARY_NOT);

      Super::writeObject(out);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_unary_not
//
ObjectExpression::Reference ObjectExpression::create_unary_not(
   OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_UnaryNot(expr, pos));
}

//
// ObjectExpression::create_unary_not
//
ObjectExpression::Reference ObjectExpression::create_unary_not(std::istream *in)
{
   return static_cast<Reference>(new ObjectExpression_UnaryNot(in));
}

// EOF

