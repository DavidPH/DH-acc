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
// ObjectExpression handling of relational and equality operators.
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../object_io.hpp"
#include "../SourceException.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_BinaryCmp
//
class ObjectExpression_BinaryCmp : public ObjectExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_BinaryCmp,
                                   ObjectExpression_Binary);

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


   //
   // ::ObjectExpression_BinaryCmp
   //
   ObjectExpression_BinaryCmp(OBJEXP_EXPRBIN_PARM, CmpType _ct)
    : Super(OBJEXP_EXPRBIN_PASS), ct(_ct)
   {
   }

   //
   // ::ObjectExpression_BinaryCmp
   //
   ObjectExpression_BinaryCmp(std::istream *in) : Super(in)
   {
      read_object(in, &ct);
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

      return Super::resolveInt();
   }


   friend void read_object(std::istream *in, CmpType *out);

   friend void write_object(std::ostream *out, CmpType const *in);

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream *out) const
   {
      write_object(out, OT_BINARY_CMP);

      Super::writeObject(out);

      write_object(out, &ct);
   }

private:
   int doCmp() const
   {
      switch (Super::getType())
      {
      case ET_ARRAY:
      case ET_OCODE:
      case ET_STRUCT:
         ERROR_NP("bad ET");

      case ET_FLOAT:
      {
         bigreal l = exprL->resolveFloat();
         bigreal r = exprR->resolveFloat();
         if (l < r) return -1;
         if (l > r) return +1;
         return 0;
      }

      case ET_INT:
      {
         bigsint l = exprL->resolveInt();
         bigsint r = exprR->resolveInt();
         if (l < r) return -1;
         if (l > r) return +1;
         return 0;
      }
      }

      ERROR_NP("bad ET");
   }

   CmpType ct;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_binary_cmp
//
ObjectExpression::Reference ObjectExpression::create_binary_cmp(
   std::istream *in)
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(in));
}

//
// ObjectExpression::create_binary_cmp_ge
//
ObjectExpression::Reference ObjectExpression::create_binary_cmp_ge(
   OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(
      exprL, exprR, pos, ObjectExpression_BinaryCmp::CMP_GE));
}

//
// ObjectExpression::create_binary_cmp_gt
//
ObjectExpression::Reference ObjectExpression::create_binary_cmp_gt(
   OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(
      exprL, exprR, pos, ObjectExpression_BinaryCmp::CMP_GT));
}

//
// ObjectExpression::create_binary_cmp_le
//
ObjectExpression::Reference ObjectExpression::create_binary_cmp_le(
   OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(
      exprL, exprR, pos, ObjectExpression_BinaryCmp::CMP_LE));
}

//
// ObjectExpression::create_binary_cmp_lt
//
ObjectExpression::Reference ObjectExpression::create_binary_cmp_lt(
   OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(
      exprL, exprR, pos, ObjectExpression_BinaryCmp::CMP_LT));
}

//
// ObjectExpression::create_binary_cmp_eq
//
ObjectExpression::Reference ObjectExpression::create_binary_cmp_eq(
   OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(
      exprL, exprR, pos, ObjectExpression_BinaryCmp::CMP_EQ));
}

//
// ObjectExpression::create_binary_cmp_ne
//
ObjectExpression::Reference ObjectExpression::create_binary_cmp_ne(
   OBJEXP_EXPRBIN_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_BinaryCmp(
      exprL, exprR, pos, ObjectExpression_BinaryCmp::CMP_NE));
}

//
// read_object<ObjectExpression_BinaryCmp::CmpType>
//
void read_object(std::istream *in, ObjectExpression_BinaryCmp::CmpType *out)
{
   *out = static_cast<ObjectExpression_BinaryCmp::CmpType>(read_object_int(in));

   if (*out > ObjectExpression_BinaryCmp::CMP_NE)
      *out = ObjectExpression_BinaryCmp::CMP_NE;
}

//
// write_object<ObjectExpression_BinaryCmp::CmpType>
//
void write_object(std::ostream *out, ObjectExpression_BinaryCmp::CmpType const *in)
{
   write_object_int(out, static_cast<bigsint>(*in));
}

// EOF

