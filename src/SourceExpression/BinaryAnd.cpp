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
// SourceExpression handling of "operator &" and "operator &=".
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
// SourceExpression_BinaryAnd
//
class SourceExpression_BinaryAnd : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryAnd,
                                   SourceExpression_Binary);

public:
   //
   // ::SourceExpression_BinaryAnd
   //
   SourceExpression_BinaryAnd(bool _assign, SRCEXP_EXPRBIN_PARM)
    : Super(NULL, NULL, SRCEXP_EXPRBIN_PASS), assign(_assign)
   {
      CONSTRUCTOR_TYPE_VARS
      CONSTRUCTOR_ARRAY_DECAY

      CONSTRAINT_INTEGER("&")
   }

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      EVALUATE_OBJECTS();
      return ObjectExpression::create_binary_and(objL, objR, position);
   }

private:
   //
   // ::doAssign
   //
   void doAssign(ObjectVector *objects, VariableData *dst)
   {
      ASSIGN_BITWISE_VARS

      ASSIGN_GET_OCODE_BITWISE(AND)

      doAssignBase(objects, dst, src, ocodeOp, ocodeGet);
   }

   //
   // ::doEvaluate
   //
   void doEvaluate(ObjectVector *objects, VariableData *dst)
   {
      EVALUATE_BITWISE_VARS(BITWISE_AND)

      if (bt == VariableType::BT_LLONG || bt == VariableType::BT_ULLONG)
         doEvaluateBaseLLB(objects, dst, src, OCODE_SETOP_AND_TEMP);
      else
         doEvaluateBase(objects, dst, src, ocode);
   }

   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      if (assign)
         doAssign(objects, dst);
      else
         doEvaluate(objects, dst);
   }

   bool assign;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_and
//
SRCEXP_EXPRBIN_DEFN(and)
{
   return new SourceExpression_BinaryAnd
              (false, exprL, exprR, context, position);
}

//
// SourceExpression::create_binary_and_eq
//
SRCEXP_EXPRBIN_DEFN(and_eq)
{
   return new SourceExpression_BinaryAnd
              (true, exprL, exprR, context, position);
}

// EOF

