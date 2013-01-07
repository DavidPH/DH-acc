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
   CounterPreambleNoClone(ObjectExpression_UnaryAdd, ObjectExpression_Unary);

public:
   ObjectExpression_UnaryAdd(OBJEXP_EXPRUNA_PARM) : Super(OBJEXP_EXPRUNA_PASS) {}
   ObjectExpression_UnaryAdd(ObjectArchive &arc) : Super(arc) {}

   bigreal resolveFIX() const {return +expr->resolveFIX();}
   bigreal resolveFLT() const {return +expr->resolveFLT();}
   bigsint resolveINT() const {return +expr->resolveINT();}
   biguint resolveUNS() const {return +expr->resolveUNS();}

protected:
   //
   // archive
   //
   virtual ObjectArchive &archive(ObjectArchive &arc)
   {
      return Super::archive(arc << OT_UNARY_ADD);
   }

private:
   //
   // writeACSPLong
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
ObjectExpression::Reference ObjectExpression::create_unary_add(OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_UnaryAdd(expr, pos));
}

//
// ObjectExpression::CreateUnaryAdd
//
ObjectExpression::Reference ObjectExpression::CreateUnaryAdd(ObjectArchive &arc)
{
   return static_cast<Reference>(new ObjectExpression_UnaryAdd(arc));
}

// EOF

