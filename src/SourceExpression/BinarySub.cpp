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
// SourceExpression handling of "operator -" and "operator -=".
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
// SourceExpression_BinarySub
//
class SourceExpression_BinarySub : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinarySub,
                                   SourceExpression_Binary);

public:
   SourceExpression_BinarySub(bool assign, SRCEXP_EXPRBIN_ARGS);

   virtual CounterPointer<ObjectExpression> makeObject() const;

private:
   //
   // ::doAssign
   //
   void doAssign(ObjectVector *objects, VariableData *dst)
   {
      ASSIGN_ARITHMETIC_VARS

      ASSIGN_GET_OCODE_ARITHMETIC(SUB)

      doAssignBase(objects, dst, src, ocodeOp, ocodeGet);
   }

   //
   // ::doEvaluate
   //
   void doEvaluate(ObjectVector *objects, VariableData *dst)
   {
      EVALUATE_ARITHMETIC_VARS(SUB)

      // TODO: X - 0

      if (bt == VariableType::BT_LLONG || bt == VariableType::BT_ULLONG)
         doEvaluateBaseLLAS(objects, dst, src, false);
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
// SourceExpression::create_binary_sub
//
SourceExpression::Pointer SourceExpression::create_binary_sub
(SRCEXP_EXPRBIN_PARM)
{
   SourceExpression::Pointer exprL = _exprL;
   SourceExpression::Pointer exprR = _exprR;

   SourceContext *&context = _context;
   SourcePosition const &position = _position;

   CONSTRUCTOR_TYPE_VARS
   CONSTRUCTOR_ARRAY_DECAY

   SourceExpression::Pointer expr =
      new SourceExpression_BinarySub(false, exprL, exprR, context, position);

   // Slight hack for pointer arithmetic.
   if (btL == VariableType::BT_POINTER && btR == VariableType::BT_POINTER)
   {
      expr = create_value_cast_implicit
             (expr, VariableType::get_bt_int(), context, position);

      bigsint retnSize = typeL->getReturn()->getSize(position);
      SourceExpression::Pointer retnExpr =
         create_value_int(retnSize, context, position);

      if (retnSize != 1)
         expr = create_binary_div(expr, retnExpr, context, position);
   }

   return expr;
}

//
// SourceExpression::create_binary_sub_eq
//
SRCEXP_EXPRBIN_DEFN(sub_eq)
{
   return new SourceExpression_BinarySub
              (true, exprL, exprR, context, position);
}

//
// SourceExpression_BinarySub::SourceExpression_BinarySub
//
SourceExpression_BinarySub::SourceExpression_BinarySub
(bool _assign, SRCEXP_EXPRBIN_PARM)
 : Super(NULL, NULL, SRCEXP_EXPRBIN_PASS), assign(_assign)
{
   CONSTRUCTOR_TYPE_VARS
   CONSTRUCTOR_ARRAY_DECAY

   // Type constraints.
   if (btL == VariableType::BT_POINTER || btR == VariableType::BT_POINTER)
   {
      // Pointer constraints.
      if (btR != VariableType::BT_POINTER && !VariableType::is_bt_integer(btR))
         throw SourceException("pointer - non-integer", position, getClassName());

      if (btL != VariableType::BT_POINTER)
         throw SourceException("non-pointer - pointer", position, getClassName());
   }
   else
   {
      CONSTRAINT_ARITHMETIC("-")
   }

   if (assign && !VariableType::is_bt_arithmetic(btR))
      throw SourceException("X -= non-arithmetic", position, getClassName());

   CONSTRUCTOR_POINTER_PREAMBLE

   if (btL == VariableType::BT_POINTER && btR != VariableType::BT_POINTER)
      exprR = create_binary_mul(exprR, exprSize, context, position);
}

//
// SourceExpression_BinarySub::makeObject
//
ObjectExpression::Pointer SourceExpression_BinarySub::makeObject() const
{
   return ObjectExpression::create_binary_sub
          (exprL->makeObject(), exprR->makeObject(), position);
}

//
// SourceExpression_BinarySub::virtual_makeObjects
//
void SourceExpression_BinarySub::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   if (assign)
      doAssign(objects, dst);
   else
      doEvaluate(objects, dst);
}

// EOF

