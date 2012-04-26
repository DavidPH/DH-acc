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
// ObjectExpression handling of symbols.
//
//-----------------------------------------------------------------------------

#include "../ObjectExpression.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"
#include "../BinaryTokenPPACS.hpp"
#include "../object_io.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_ValueSymbol
//
class ObjectExpression_ValueSymbol : public ObjectExpression
{
   MAKE_COUNTER_CLASS_BASE(ObjectExpression_ValueSymbol, ObjectExpression);

public:
   //
   // ::ObjectExpression_ValueSymbol
   //
   ObjectExpression_ValueSymbol
   (std::string const &_value, SourcePosition const &_pos)
    : Super(_pos), value(_value)
   {
   }

   //
   // ::ObjectExpression_ValueSymbol
   //
   ObjectExpression_ValueSymbol(std::istream *in) : Super(in)
   {
      read_object(in, &value);
   }

   //
   // ::canResolve
   //
   virtual bool canResolve() const
   {
      ObjectExpression::Pointer symbol =
         ObjectExpression::get_symbol_null(value);

      return symbol && symbol->canResolve();
   }

   //
   // ::getType
   //
   virtual ExpressionType getType() const
   {
      return ObjectExpression::get_symbol_type(value, position);
   }

   //
   // ::resolveFloat
   //
   virtual bigreal resolveFloat() const
   {
      return ObjectExpression::get_symbol(value, position)->resolveFloat();
   }

   //
   // ::resolveInt
   //
   virtual bigsint resolveInt() const
   {
      return ObjectExpression::get_symbol(value, position)->resolveInt();
   }

   //
   // ::resolveSymbol
   //
   virtual std::string resolveSymbol() const
   {
      return value;
   }

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream *out) const
   {
      write_object(out, OT_VALUE_SYMBOL);

      Super::writeObject(out);

      write_object(out, &value);
   }

private:
   //
   // ::writeACSPLong
   //
   virtual void writeACSPLong(std::ostream *out) const
   {
      BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_SYMBOL);
      BinaryTokenPPACS::write_ACSP_string(out, value);
   }

   std::string value;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_value_symbol
//
ObjectExpression::Pointer ObjectExpression::create_value_symbol
(std::string const &value, SourcePosition const &pos)
{
   return new ObjectExpression_ValueSymbol(value, pos);
}

//
// ObjectExpression::create_value_symbol
//
ObjectExpression::Pointer ObjectExpression::create_value_symbol
(std::istream *in)
{
   return new ObjectExpression_ValueSymbol(in);
}

// EOF

