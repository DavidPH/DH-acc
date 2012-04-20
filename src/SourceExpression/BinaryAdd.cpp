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
// SourceExpression handling of "operator +" and "operator +=".
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryAdd
//
class SourceExpression_BinaryAdd : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryAdd,
                                     SourceExpression_Binary);

public:
   SourceExpression_BinaryAdd(bool assign, SRCEXP_EXPRBIN_ARGS);

   virtual CounterPointer<ObjectExpression> makeObject() const;

private:
   //
   // ::doAssign
   //
   void doAssign(ObjectVector *objects, VariableData *dst)
   {
      ASSIGN_ARITHMETIC_VARS

      ASSIGN_GET_OCODE_ARITHMETIC(ADD)

      doAssignBase(objects, dst, src, ocodeOp, ocodeGet);
   }

   //
   // ::doEvaluate
   //
   void doEvaluate(ObjectVector *objects, VariableData *dst)
   {
      EVALUATE_ARITHMETIC_VARS(ADD)

      // TODO: X + 0

      if (bt == VariableType::BT_LLONG || bt == VariableType::BT_ULLONG)
         doEvaluateBaseLLAS(objects, dst, src, true);
      else
      {
         ocode = static_cast<ObjectCode>(ocode + getOcodeType(bt));
         doEvaluateBase(objects, dst, src, ocode);
      }
   }

   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   bool assign;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_add
//
SRCEXP_EXPRBIN_DEFN(add)
{
   return new SourceExpression_BinaryAdd
              (false, exprL, exprR, context, position);
}

//
// SourceExpression::create_binary_add_eq
//
SRCEXP_EXPRBIN_DEFN(add_eq)
{
   return new SourceExpression_BinaryAdd
              (true, exprL, exprR, context, position);
}

//
// SourceExpression_BinaryAdd::SourceExpression_BinaryAdd
//
SourceExpression_BinaryAdd::SourceExpression_BinaryAdd
(bool _assign, SRCEXP_EXPRBIN_PARM)
 : Super(NULL, NULL, SRCEXP_EXPRBIN_PASS), assign(_assign)
{
   CONSTRUCTOR_TYPE_VARS
   CONSTRUCTOR_ARRAY_DECAY

   // Type constraints.
   if (btL == VariableType::BT_POINTER || btR == VariableType::BT_POINTER)
   {
      // Pointer constraints.
      if (btL == VariableType::BT_POINTER && btR == VariableType::BT_POINTER)
         ERROR_N(position, "pointer + pointer");

      if (btL == VariableType::BT_POINTER && !VariableType::is_bt_integer(btR))
         ERROR_N(position, "pointer + non-integer");

      if (!VariableType::is_bt_integer(btL) && btR == VariableType::BT_POINTER)
         ERROR_N(position, "non-integer + pointer");
   }
   else
   {
      CONSTRAINT_ARITHMETIC("+")
   }

   if (assign && !VariableType::is_bt_arithmetic(btR))
      ERROR_N(position, "X -= non-arithmetic");

   CONSTRUCTOR_POINTER_PREAMBLE

   if (btL == VariableType::BT_POINTER)
      exprR = create_binary_mul(exprR, exprSize, context, position);

   if (btR == VariableType::BT_POINTER)
      exprL = create_binary_mul(exprL, exprSize, context, position);
}

//
// SourceExpression_BinaryAdd::makeObject
//
ObjectExpression::Pointer SourceExpression_BinaryAdd::makeObject() const
{
   EVALUATE_OBJECTS();
   return ObjectExpression::create_binary_add(objL, objR, position);
}

//
// SourceExpression_BinaryAdd::virtual_makeObjects
//
void SourceExpression_BinaryAdd::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   if (assign)
      doAssign(objects, dst);
   else
      doEvaluate(objects, dst);
}

// EOF

