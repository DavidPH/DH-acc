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
// ObjectExpression handling of unsigned integer numbers.
//
//-----------------------------------------------------------------------------

#include "Value.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_ValueUNS
//
class ObjectExpression_ValueUNS : public ObjectExpression
{
   CounterPreambleNoClone(ObjectExpression_ValueUNS, ObjectExpression);

public:
   ObjectExpression_ValueUNS(biguint _value, ExpressionType _type, OBJEXP_EXPR_PARM)
    : Super(_pos), value(_value), type(_type) {}
   ObjectExpression_ValueUNS(ObjectArchive &arc) : Super(arc) {arc << value << type;}

   virtual bool canResolve() const {return true;}

   virtual ExpressionType getType() const {return type;}

   // HACK! Allow resolving as INT.
   virtual bigsint resolveINT() const {return value;}
   virtual biguint resolveUNS() const {return value;}

protected:
   //
   // archive
   //
   virtual ObjectArchive &archive(ObjectArchive &arc)
   {
      return Super::archive(arc << OT_VALUE_UNS) << value << type;
   }

private:
   //
   // writeACSPLong
   //
   virtual void writeACSPLong(std::ostream *out) const
   {
      BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_LITERAL);
      BinaryTokenACS::write_ACS0_32(out, value);
   }

   biguint value;
   ExpressionType type;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// CreateValueUNS*
//
CreateValueX(biguint, UNS)

// EOF

