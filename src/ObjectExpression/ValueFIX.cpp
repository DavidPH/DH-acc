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

#include "Value.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_ValueFIX
//
class ObjectExpression_ValueFIX : public ObjectExpression
{
   CounterPreambleNoClone(ObjectExpression_ValueFIX, ObjectExpression);

public:
   ObjectExpression_ValueFIX(bigreal _value, ExpressionType _type, OBJEXP_EXPR_PARM)
    : Super(_pos), value(_value), type(_type) {}
   ObjectExpression_ValueFIX(ObjectLoad &arc) : Super(arc) {arc >> value >> type;}

   virtual bool canResolve() const {return true;}

   virtual ExpressionType getType() const {return type;}

   virtual bigreal resolveFIX() const {return value;}

protected:
   //
   // save
   //
   virtual ObjectSave &save(ObjectSave &arc) const
   {
      return Super::save(arc << OT_VALUE_FIX) << value << type;
   }

private:
   bigreal value;
   ExpressionType type;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// CreateValueFIX*
//
CreateValueX(bigreal, FIX)

// EOF

