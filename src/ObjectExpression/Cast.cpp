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
// ObjectExpression handling of type-casting.
//
//-----------------------------------------------------------------------------

#include "../ObjectExpression.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_Cast
//
class ObjectExpression_Cast : public ObjectExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_Cast, ObjectExpression);

public:
   //
   // ::ObjectExpression_Cast
   //
   ObjectExpression_Cast
   (ExpressionType _dstType, ExpressionType _srcType, ObjectExpression *_expr,
    SourcePosition const &_position)
   : Super(_position), expr(_expr), dstType(_dstType), srcType(_srcType)
   {
   }

   //
   // ::ObjectExpression_Cast
   //
   ObjectExpression_Cast(std::istream *in)
   : Super(in)
   {
      read_object(in, &expr);

      read_object(in, &dstType);
      read_object(in, &srcType);
   }

   //
   // ::canResolve
   //
   virtual bool canResolve() const
   {
      return expr->canResolve();
   }

   //
   // ::getType
   //
   virtual ExpressionType getType() const
   {
      return dstType;
   }

   //
   // ::resolveFloat
   //
   virtual bigreal resolveFloat() const
   {
      if (dstType != ET_FLOAT) return Super::resolveFloat();

      switch (srcType)
      {
      case ET_ARRAY:
      case ET_OCODE:
      case ET_STRUCT:
         break;

      case ET_FLOAT:
         return expr->resolveFloat();

      case ET_INT:
         return static_cast<bigreal>(expr->resolveInt());
      }

      return Super::resolveFloat();
   }

   //
   // ::resolveInt
   //
   virtual bigsint resolveInt() const
   {
      if (dstType != ET_INT) return Super::resolveInt();

      switch (srcType)
      {
      case ET_ARRAY:
      case ET_OCODE:
      case ET_STRUCT:
         break;

      case ET_FLOAT:
         return static_cast<bigsint>(expr->resolveFloat());

      case ET_INT:
         return expr->resolveInt();
      }

      return Super::resolveInt();
   }

   //
   // ::writeACSP
   //
   virtual void writeACSP(std::ostream *out) const
   {
      if (dstType == srcType)
      {
         expr->writeACSP(out);
         return;
      }

      if (dstType == ET_FLOAT && srcType == ET_INT)
      {
         BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_F2I);
         expr->writeACSP(out);
         return;
      }

      if (dstType == ET_INT && srcType == ET_FLOAT)
      {
         BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_I2F);
         expr->writeACSP(out);
         return;
      }

      Super::writeACSP(out);
   }

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream *out) const
   {
      write_object(out, OT_CAST);

      Super::writeObject(out);

      write_object(out, expr);

      write_object(out, dstType);
      write_object(out, srcType);
   }

private:
   ObjectExpression::Pointer expr;

   ExpressionType dstType;
   ExpressionType srcType;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_cast
//
ObjectExpression::Pointer ObjectExpression::create_cast(std::istream *in)
{
   return new ObjectExpression_Cast(in);
}

//
// ObjectExpression::create_cast_float_to_int
//
ObjectExpression::Pointer ObjectExpression::
create_cast_float_to_int(ObjectExpression *expr, SourcePosition const &position)
{
   return new ObjectExpression_Cast(ET_INT, ET_FLOAT, expr, position);
}

//
// ObjectExpression::create_cast_int_to_float
//
ObjectExpression::Pointer ObjectExpression::
create_cast_int_to_float(ObjectExpression *expr, SourcePosition const &position)
{
   return new ObjectExpression_Cast(ET_FLOAT, ET_INT, expr, position);
}

// EOF

