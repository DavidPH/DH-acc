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
// ObjectExpression handling of signed integer numbers.
//
//-----------------------------------------------------------------------------

#include "../ObjectExpression.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"
#include "../object_io.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_ValueINT
//
class ObjectExpression_ValueINT : public ObjectExpression
{
   MAKE_COUNTER_CLASS_BASE(ObjectExpression_ValueINT, ObjectExpression);

public:
   //
   // ::ObjectExpression_ValueINT
   //
   ObjectExpression_ValueINT(bigsint _value, SourcePosition const &_pos)
    : Super(_pos), value(_value)
   {
   }

   //
   // ::ObjectExpression_ValueINT
   //
   ObjectExpression_ValueINT(std::istream *in) : Super(in)
   {
      read_object(in, &value);
   }

   //
   // ::canResolve
   //
   virtual bool canResolve() const
   {
      return true;
   }

   //
   // ::getType
   //
   virtual ExpressionType getType() const
   {
      return ET_INT;
   }

   //
   // ::resolveINT
   //
   virtual bigsint resolveINT() const
   {
      return value;
   }

   //
   // ::resolveUNS
   //
   // HACK! Allow resolving as UNS.
   //
   virtual biguint resolveUNS() const
   {
      return value;
   }

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream *out) const
   {
      write_object(out, OT_VALUE_INT);

      Super::writeObject(out);

      write_object(out, &value);
   }

private:
   //
   // ::writeACSPLong
   //
   virtual void writeACSPLong(std::ostream *out) const
   {
      BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_LITERAL);
      BinaryTokenACS::write_ACS0_32(out, value);
   }

   bigsint value;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_value_int
//
ObjectExpression::Reference ObjectExpression::create_value_int(bigsint value, OBJEXP_EXPR_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_ValueINT(value, pos));
}

//
// ObjectExpression::create_value_int
//
ObjectExpression::Reference ObjectExpression::create_value_int(std::istream *in)
{
   return static_cast<Reference>(new ObjectExpression_ValueINT(in));
}

// EOF

