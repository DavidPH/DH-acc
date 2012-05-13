//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
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
// SourceExpression handling of "operator |" and "operator |=".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryIOr
//
class SourceExpression_BinaryIOr : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryIOr,
                                   SourceExpression_Binary);

public:
   //
   // ::SourceExpression_BinaryIOr
   //
   SourceExpression_BinaryIOr(SRCEXP_EXPRBIN_PARM, bool _assign)
    : Super(SRCEXP_EXPRBIN_PASS, NULL, NULL, _assign)
   {
      CONSTRUCTOR_TYPE_VARS();
      CONSTRUCTOR_ARRAY_DECAY();

      CONSTRAINT_INTEGER("|");
   }

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      EVALUATE_OBJECTS();
      return ObjectExpression::create_binary_ior(objL, objR, pos);
   }

protected:
   //
   // ::doGet
   //
   virtual void doGet(ObjectVector *objects, VariableType *type, int tmpBase)
   {
      switch (type->getBasicType())
      {
         DO_GET_CASES(IOR, BITWISE_, 32, 32, 32);

      case VariableType::BT_INT_LL: doGetBaseILLB(objects, type, tmpBase, OCODE_BITWISE_IOR32); break;
      case VariableType::BT_UNS_LL: doGetBaseILLB(objects, type, tmpBase, OCODE_BITWISE_IOR32); break;
      }
   }

   //
   // ::doSet
   //
   virtual bool doSet(ObjectVector *objects, VariableData *data,
                      VariableType *type, int)
   {
      DO_SET_SWITCHES(IOR, 32, 32, 32, ACS);
      return false;
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_ior
//
SRCEXP_EXPRBIN_DEFN(ior)
{
   return new SourceExpression_BinaryIOr(exprL, exprR, context, pos, false);
}

//
// SourceExpression::create_binary_ior_eq
//
SRCEXP_EXPRBIN_DEFN(ior_eq)
{
   return new SourceExpression_BinaryIOr(exprL, exprR, context, pos, true);
}

// EOF

