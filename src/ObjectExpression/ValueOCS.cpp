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
// ObjectExpression handling of object-codes.
//
//-----------------------------------------------------------------------------

#include "../ObjectExpression.hpp"

#include "../ObjectCode.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_ValueOCS
//
class ObjectExpression_ValueOCS : public ObjectExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_ValueOCS, ObjectExpression);

public:
   ObjectExpression_ValueOCS(ObjectCodeSet const &_value, OBJEXP_EXPR_PARM)
    : Super(OBJEXP_EXPR_PASS), value(_value) {}
   ObjectExpression_ValueOCS(ObjectArchive &arc) : Super(arc) {arc << value;}

   virtual bool canResolve() const {return true;}

   virtual ExpressionType getType() const {return ET_OCS;}

   virtual ObjectCodeSet resolveOCS() const {return value;}

protected:
   //
   // archive
   //
   virtual ObjectArchive &archive(ObjectArchive &arc)
   {
      return Super::archive(arc << OT_VALUE_OCS) << value;
   }

private:
   ObjectCodeSet value;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_value_ocs
//
ObjectExpression::Reference ObjectExpression::create_value_ocs(
   ObjectCodeSet const &value, OBJEXP_EXPR_ARGS)
{
   return static_cast<Reference>(new ObjectExpression_ValueOCS(value, pos));
}

//
// ObjectExpression::CreateValueOCS
//
ObjectExpression::Reference ObjectExpression::CreateValueOCS(ObjectArchive &arc)
{
   return static_cast<Reference>(new ObjectExpression_ValueOCS(arc));
}

// EOF

