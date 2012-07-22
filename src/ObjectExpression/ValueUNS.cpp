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
// ObjectExpression handling of unsigned integer numbers.
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
// ObjectExpression_ValueUNS
//
class ObjectExpression_ValueUNS : public ObjectExpression
{
   MAKE_COUNTER_CLASS_BASE(ObjectExpression_ValueUNS, ObjectExpression);

public:
   //
   // ::ObjectExpression_ValueUNS
   //
   ObjectExpression_ValueUNS(biguint _value, SourcePosition const &_pos)
    : Super(_pos), value(_value)
   {
   }

   //
   // ::ObjectExpression_ValueUNS
   //
   ObjectExpression_ValueUNS(std::istream *in) : Super(in)
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
      return ET_UNS;
   }

   //
   // ::resolveINT
   //
   // HACK! Allow resolving as INT.
   //
   virtual bigsint resolveINT() const
   {
      return value;
   }

   //
   // ::resolveUNS
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
      write_object(out, OT_VALUE_UNS);

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

   biguint value;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_value_uns
//
ObjectExpression::Reference ObjectExpression::create_value_uns(biguint value, OBJEXP_EXPR_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_ValueUNS(value, pos));
}

//
// ObjectExpression::create_value_uns
//
ObjectExpression::Reference ObjectExpression::create_value_uns(std::istream *in)
{
   return static_cast<Reference>(new ObjectExpression_ValueUNS(in));
}

// EOF

