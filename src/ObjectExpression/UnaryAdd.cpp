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
// ObjectExpression handling of "operator +".
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_UnaryAdd
//
class ObjectExpression_UnaryAdd : public ObjectExpression_Unary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_UnaryAdd,
                                   ObjectExpression_Unary);

public:
   //
   // ::ObjectExpression_UnaryAdd
   //
   ObjectExpression_UnaryAdd(OBJEXP_EXPRUNA_PARM) : Super(OBJEXP_EXPRUNA_PASS)
   {
   }

   //
   // ::ObjectExpression_UnaryAdd
   //
   ObjectExpression_UnaryAdd(std::istream *in) : Super(in)
   {
   }

   //
   // ::resolveFloat
   //
   virtual bigreal resolveFloat() const
   {
      return expr->resolveFloat();
   }

   //
   // ::resolveInt
   //
   virtual bigsint resolveInt() const
   {
      return expr->resolveInt();
   }

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream *out) const
   {
      write_object(out, OT_UNARY_ADD);

      Super::writeObject(out);
   }

private:
   //
   // ::writeACSPLong
   //
   virtual void writeACSPLong(std::ostream *out) const
   {
      expr->writeACSP(out);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_unary_add
//
ObjectExpression::Reference ObjectExpression::create_unary_add(
   OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_UnaryAdd(expr, pos));
}

//
// ObjectExpression::create_unary_add
//
ObjectExpression::Reference ObjectExpression::create_unary_add(std::istream *in)
{
   return static_cast<Reference>(new ObjectExpression_UnaryAdd(in));
}

// EOF

