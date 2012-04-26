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
// ObjectExpression handling of floating-point numbers.
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
// ObjectExpression_ValueFloat
//
class ObjectExpression_ValueFloat : public ObjectExpression
{
   MAKE_COUNTER_CLASS_BASE(ObjectExpression_ValueFloat, ObjectExpression);

public:
   //
   // ::ObjectExpression_ValueFloat
   //
   ObjectExpression_ValueFloat(bigreal _value, SourcePosition const &_pos)
    : Super(_pos), value(_value)
   {
   }

   //
   // ::ObjectExpression_ValueFloat
   //
   ObjectExpression_ValueFloat(std::istream *in) : Super(in)
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
      return ET_FLOAT;
   }

   //
   // ::resolveFloat
   //
   virtual bigreal resolveFloat() const
   {
      return value;
   }

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream * out) const
   {
      write_object(out, OT_VALUE_FLOAT);

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
      BinaryTokenACS::write_ACS0_32(out, *this);
   }

   bigreal value;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_value_float
//
ObjectExpression::Pointer ObjectExpression::create_value_float
(bigreal value, SourcePosition const &pos)
{
   return new ObjectExpression_ValueFloat(value, pos);
}

//
// ObjectExpression::create_value_float
//
ObjectExpression::Pointer ObjectExpression::create_value_float(std::istream *in)
{
   return new ObjectExpression_ValueFloat(in);
}

// EOF

