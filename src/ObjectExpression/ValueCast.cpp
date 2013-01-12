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
// ObjectExpression handling of type-casting.
//
//-----------------------------------------------------------------------------

#include "Unary.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"
#include "../SourceException.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_ValueCast
//
class ObjectExpression_ValueCast : public ObjectExpression_Unary
{
   CounterPreambleNoClone(ObjectExpression_ValueCast, ObjectExpression_Unary);

public:
   ObjectExpression_ValueCast(ExpressionType _type, OBJEXP_EXPRUNA_PARM)
    : Super(OBJEXP_EXPRUNA_PASS), type(_type) {}
   ObjectExpression_ValueCast(ObjectArchive &arc) : Super(arc) {arc << type;}

   virtual bool canResolve() const {return expr->canResolve();}

   virtual ExpressionType getType() const {return type;}

   //
   // resolveFIX
   //
   virtual bigreal resolveFIX() const
   {
      bigreal value = resolveExpr<bigreal>();

      switch(type)
      {
      case ET_FIX_HH:
      case ET_FIX_H:
      case ET_FIX:
      case ET_FIX_L:
      case ET_FIX_LL:
         return value;

      default:
         return Super::resolveFIX();
      }
   }

   //
   // resolveFLT
   //
   virtual bigreal resolveFLT() const
   {
      bigreal value = resolveExpr<bigreal>();

      switch(type)
      {
      case ET_FLT_HH:
      case ET_FLT_H:
      case ET_FLT:
      case ET_FLT_L:
      case ET_FLT_LL:
         return value;

      default:
         return Super::resolveFLT();
      }
   }

   //
   // resolveINT
   //
   virtual bigsint resolveINT() const
   {
      bigsint value = resolveExpr<bigsint>();

      switch(type)
      {
      // HACK! Allow INT for UNS.
      case ET_UNS_HH:
      case ET_UNS_H:
      case ET_UNS:
      case ET_UNS_L:
      case ET_UNS_LL:

      case ET_INT_HH:
      case ET_INT_H:
      case ET_INT:
      case ET_INT_L:
      case ET_INT_LL:
         return value;

      default:
         return Super::resolveINT();
      }
   }

   //
   // resolveUNS
   //
   virtual biguint resolveUNS() const
   {
      // Yes, as bigsint. Better for casting from bigreal.
      biguint value = resolveExpr<bigsint>();

      switch(type)
      {
      // HACK! Allow UNS for INT.
      case ET_INT_HH:
      case ET_INT_H:
      case ET_INT:
      case ET_INT_L:
      case ET_INT_LL:

      case ET_UNS_HH:
      case ET_UNS_H:
      case ET_UNS:
      case ET_UNS_L:
      case ET_UNS_LL:
         return value;

      default:
         return Super::resolveUNS();
      }
   }

protected:
   //
   // archive
   //
   virtual ObjectArchive &archive(ObjectArchive &arc)
   {
      return Super::archive(arc << OT_VALUE_CAST) << type;
   }

private:
   template<typename T> T resolveExpr() const
   {
      switch(expr->getType())
      {
      case ET_FIX_HH:
      case ET_FIX_H:
      case ET_FIX:
      case ET_FIX_L:
      case ET_FIX_LL:
         return static_cast<T>(expr->resolveFIX());

      case ET_FLT_HH:
      case ET_FLT_H:
      case ET_FLT:
      case ET_FLT_L:
      case ET_FLT_LL:
         return static_cast<T>(expr->resolveFLT());

      case ET_INT_HH:
      case ET_INT_H:
      case ET_INT:
      case ET_INT_L:
      case ET_INT_LL:
         return static_cast<T>(expr->resolveINT());

      case ET_UNS_HH:
      case ET_UNS_H:
      case ET_UNS:
      case ET_UNS_L:
      case ET_UNS_LL:
         return static_cast<T>(expr->resolveUNS());

      case ET_OCS: break;

      case ET_ARR: break;
      case ET_MAP: break;
      }

      Error_NP("bad expr for cast source");
   }

   ExpressionType type;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::CreateValueCast
//
ObjectExpression::Reference ObjectExpression::CreateValueCast(ObjectArchive &arc)
{
   return static_cast<Reference>(new ObjectExpression_ValueCast(arc));
}

//
// ObjectExpression::CreateValueCast
//
ObjectExpression::Reference ObjectExpression::CreateValueCast(
   ExpressionType type, OBJEXP_EXPRUNA_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_ValueCast(type, expr, pos));
}

// EOF

