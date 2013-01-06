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
// ObjectExpression handling of fixed-point numbers.
//
//-----------------------------------------------------------------------------

#include "../ObjectExpression.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"
#include "../ObjectArchive.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_ValueFIX
//
class ObjectExpression_ValueFIX : public ObjectExpression
{
   MAKE_COUNTER_CLASS_BASE(ObjectExpression_ValueFIX, ObjectExpression);

public:
   ObjectExpression_ValueFIX(bigreal _value, SourcePosition const &_pos)
    : Super(_pos), value(_value) {}
   ObjectExpression_ValueFIX(ObjectArchive &arc) : Super(arc) {arc << value;}

   virtual bool canResolve() const {return true;}

   virtual ExpressionType getType() const {return ET_FIX;}

   virtual bigreal resolveFIX() const {return value;}

protected:
   //
   // archive
   //
   virtual ObjectArchive &archive(ObjectArchive &arc)
   {
      return Super::archive(arc << OT_VALUE_FIX) << value;
   }

private:
   //
   // writeACSPLong
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
// ObjectExpression::create_value_fix
//
ObjectExpression::Reference ObjectExpression::create_value_fix(bigreal value, OBJEXP_EXPR_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_ValueFIX(value, pos));
}

//
// ObjectExpression::CreateValueFIX
//
ObjectExpression::Reference ObjectExpression::CreateValueFIX(ObjectArchive &arc)
{
   return static_cast<Reference>(new ObjectExpression_ValueFIX(arc));
}

// EOF

