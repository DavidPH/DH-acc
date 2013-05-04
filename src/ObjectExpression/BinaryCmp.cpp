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
// ObjectExpression handling of relational and equality operators.
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ObjectArchive.hpp"
#include "../SourceException.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_BinaryCmp
//
class ObjectExpression_BinaryCmp : public ObjectExpression_Binary
{
   CounterPreambleNoClone(ObjectExpression_BinaryCmp, ObjectExpression_Binary);

public:
   //
   // ::CmpType
   //
   enum CmpType
   {
      CMP_GE,
      CMP_GT,
      CMP_LE,
      CMP_LT,
      CMP_EQ,
      CMP_NE,
   };


   ObjectExpression_BinaryCmp(OBJEXP_EXPRBIN_PARM, CmpType _ct)
    : Super{OBJEXP_EXPRBIN_PASS}, ct{_ct} {}
   ObjectExpression_BinaryCmp(ObjectLoad &arc) : Super{arc} {arc >> ct;}

   virtual ExpressionType getType() const {return ET_INT;}

   //
   // ::resolveINT
   //
   virtual bigsint resolveINT() const
   {
      int cmp = doCmp();
      switch (ct)
      {
      case CMP_GE: return cmp >= 0;
      case CMP_GT: return cmp >  0;
      case CMP_LE: return cmp <= 0;
      case CMP_LT: return cmp <  0;
      case CMP_EQ: return cmp == 0;
      case CMP_NE: return cmp != 0;
      }

      return Super::resolveINT();
   }


   friend ObjectSave &operator << (ObjectSave &arc, CmpType const &data);

   friend ObjectLoad &operator >> (ObjectLoad &arc, CmpType &data);

protected:
   //
   // save
   //
   virtual ObjectSave &save(ObjectSave &arc) const
   {
      return Super::save(arc << OT_BINARY_CMP) << ct;
   }

private:
   //
   // doCmp
   //
   int doCmp() const
   {
      switch (Super::getType())
      {
      case ET_FIX_HH:
      case ET_FIX_H:
      case ET_FIX:
      case ET_FIX_L:
      case ET_FIX_LL:
      {
         bigreal l = exprL->resolveFIX();
         bigreal r = exprR->resolveFIX();
         if(l < r) return -1;
         if(l > r) return +1;
         return 0;
      }

      case ET_FLT_HH:
      case ET_FLT_H:
      case ET_FLT:
      case ET_FLT_L:
      case ET_FLT_LL:
      {
         bigreal l = exprL->resolveFLT();
         bigreal r = exprR->resolveFLT();
         if(l < r) return -1;
         if(l > r) return +1;
         return 0;
      }

      case ET_INT_HH:
      case ET_INT_H:
      case ET_INT:
      case ET_INT_L:
      case ET_INT_LL:
      {
         bigsint l = exprL->resolveINT();
         bigsint r = exprR->resolveINT();
         if(l < r) return -1;
         if(l > r) return +1;
         return 0;
      }

      case ET_UNS_HH:
      case ET_UNS_H:
      case ET_UNS:
      case ET_UNS_L:
      case ET_UNS_LL:
      {
         biguint l = exprL->resolveUNS();
         biguint r = exprR->resolveUNS();
         if(l < r) return -1;
         if(l > r) return +1;
         return 0;
      }

      case ET_OCS:
      case ET_ARR:
      case ET_MAP:
         Error_NP("bad ET");
      }

      Error_NP("bad ET");
   }

   CmpType ct;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_binary_cmp_ge
//
ObjectExpression::Reference ObjectExpression::create_binary_cmp_ge(OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(
      exprL, exprR, pos, ObjectExpression_BinaryCmp::CMP_GE));
}

//
// ObjectExpression::create_binary_cmp_gt
//
ObjectExpression::Reference ObjectExpression::create_binary_cmp_gt(OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(
      exprL, exprR, pos, ObjectExpression_BinaryCmp::CMP_GT));
}

//
// ObjectExpression::create_binary_cmp_le
//
ObjectExpression::Reference ObjectExpression::create_binary_cmp_le(OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(
      exprL, exprR, pos, ObjectExpression_BinaryCmp::CMP_LE));
}

//
// ObjectExpression::create_binary_cmp_lt
//
ObjectExpression::Reference ObjectExpression::create_binary_cmp_lt(OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(
      exprL, exprR, pos, ObjectExpression_BinaryCmp::CMP_LT));
}

//
// ObjectExpression::create_binary_cmp_eq
//
ObjectExpression::Reference ObjectExpression::create_binary_cmp_eq(OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(
      exprL, exprR, pos, ObjectExpression_BinaryCmp::CMP_EQ));
}

//
// ObjectExpression::create_binary_cmp_ne
//
ObjectExpression::Reference ObjectExpression::create_binary_cmp_ne(OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(
      exprL, exprR, pos, ObjectExpression_BinaryCmp::CMP_NE));
}

//
// ObjectExpression::LoadBinaryCmp
//
auto ObjectExpression::LoadBinaryCmp(ObjectLoad &arc) -> Reference
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(arc));
}

//
// operator ObjectSave << ObjectExpression_BinaryCmp::CmpType
//
ObjectSave &operator << (ObjectSave &arc, ObjectExpression_BinaryCmp::CmpType const &data)
{
   return arc.saveEnum(data);
}

//
// operator ObjectLoad >> ObjectExpression_BinaryCmp::CmpType
//
ObjectLoad &operator >> (ObjectLoad &arc, ObjectExpression_BinaryCmp::CmpType &data)
{
   return arc.loadEnum(data, ObjectExpression_BinaryCmp::CMP_NE);
}

// EOF

