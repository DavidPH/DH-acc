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
// SourceExpression handling of "operator ^" and "operator ^=".
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
// SourceExpression_BinaryXOr
//
class SourceExpression_BinaryXOr : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryXOr,
                                   SourceExpression_Binary);

public:
   SourceExpression_BinaryXOr(bool assign, SRCEXP_EXPRBIN_ARGS);

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      EVALUATE_OBJECTS();
      return ObjectExpression::create_binary_xor(objL, objR, pos);
   }

private:
   //
   // ::doAssign
   //
   void doAssign(ObjectVector *objects, VariableData *dst)
   {
      ASSIGN_BITWISE_VARS

      ASSIGN_GET_OCODE_BITWISE(XOR)

      doAssignBase(objects, dst, src, ocodeOp, ocodeGet);
   }

   //
   // ::doEvaluate
   //
   void doEvaluate(ObjectVector *objects, VariableData *dst)
   {
      EVALUATE_BITWISE_VARS(BITWISE_XOR)

      if (bt == VariableType::BT_LLONG || bt == VariableType::BT_ULLONG)
         doEvaluateBaseLLB(objects, dst, src, OCODE_SETOP_XOR_TEMP);
      else
         doEvaluateBase(objects, dst, src, ocode);
   }

   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   bool assign;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_xor
//
SRCEXP_EXPRBIN_DEFN(xor)
{
   return new SourceExpression_BinaryXOr(false, exprL, exprR, context, pos);
}

//
// SourceExpression::create_binary_xor_eq
//
SRCEXP_EXPRBIN_DEFN(xor_eq)
{
   return new SourceExpression_BinaryXOr(true, exprL, exprR, context, pos);
}

//
// SourceExpression_BinaryXOr::SourceExpression_BinaryXOr
//
SourceExpression_BinaryXOr::SourceExpression_BinaryXOr
(bool _assign, SRCEXP_EXPRBIN_PARM)
 : Super(NULL, NULL, SRCEXP_EXPRBIN_PASS), assign(_assign)
{
   CONSTRUCTOR_TYPE_VARS
   CONSTRUCTOR_ARRAY_DECAY

   CONSTRAINT_INTEGER("^")
}

//
// SourceExpression_BinaryXOr::virtual_makeObjects
//
void SourceExpression_BinaryXOr::virtual_makeObjects(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   if (assign)
      doAssign(objects, dst);
   else
      doEvaluate(objects, dst);
}

// EOF

