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
// ObjectExpression handling of "operator &&".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ACSP.hpp"
#include "../BinaryTokenACS.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ObjectExpression_BranchXOr
//
class ObjectExpression_BranchXOr : public ObjectExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(ObjectExpression_BranchXOr,
                                   ObjectExpression_Binary);

public:
   //
   // ::ObjectExpression_BranchXOr
   //
   ObjectExpression_BranchXOr
   (ObjectExpression *_exprL, ObjectExpression *_exprR,
    SourcePosition const &_position)
    : Super(_exprL, _exprR, _position)
   {
   }

   //
   // ::ObjectExpression_BranchXOr
   //
   ObjectExpression_BranchXOr(std::istream *in)
   : Super(in)
   {
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
      switch (exprL->getType())
      {
      case ET_ARRAY:
      case ET_OCODE:
      case ET_STRUCT:
         break;

      case ET_FLOAT:
         return !!exprL->resolveFloat() ^ !!exprR->resolveFloat();

      case ET_INT:
         return !!exprL->resolveInt() ^ !!exprR->resolveInt();
      }

      return Super::resolveInt();
   }

   //
   // ::writeACSP
   //
   virtual void writeACSP(std::ostream *out) const
   {
      BinaryTokenACS::write_ACS0_32(out, ACSP_EXPR_LXOR);

      exprL->writeACSP(out);
      exprR->writeACSP(out);
   }

protected:
   //
   // ::writeObject
   //
   virtual void writeObject(std::ostream *out) const
   {
      write_object(out, OT_BRANCH_XOR);

      Super::writeObject(out);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectExpression::create_branch_xor
//
ObjectExpression::Pointer ObjectExpression::
create_branch_xor(ObjectExpression *exprL, ObjectExpression *exprR,
                  SourcePosition const &position)
{
   return new ObjectExpression_BranchXOr(exprL, exprR, position);
}

//
// ObjectExpression::create_branch_and
//
ObjectExpression::Pointer ObjectExpression::create_branch_xor(std::istream *in)
{
   return new ObjectExpression_BranchXOr(in);
}

// EOF

