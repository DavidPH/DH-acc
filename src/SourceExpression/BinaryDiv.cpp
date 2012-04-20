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
// SourceExpression handling of "operator /" and "operator "/=".
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
// SourceExpression_BinaryDiv
//
class SourceExpression_BinaryDiv : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryDiv,
                                   SourceExpression_Binary);

public:
   SourceExpression_BinaryDiv(bool assign, SRCEXP_EXPRBIN_ARGS);

   virtual CounterPointer<ObjectExpression> makeObject() const;

private:
   //
   // ::doAssign
   //
   void doAssign(ObjectVector *objects, VariableData *dst)
   {
      ASSIGN_ARITHMETIC_VARS

      ASSIGN_GET_OCODE_ARITHMETIC(DIV)

      doAssignBase(objects, dst, src, ocodeOp, ocodeGet);
   }

   //
   // ::doEvaluate
   //
   void doEvaluate(ObjectVector *objects, VariableData *dst)
   {
      EVALUATE_ARITHMETIC_VARS(DIV)

      ocode = static_cast<ObjectCode>(ocode + getOcodeType(bt));

      // TODO: X / 1
      // TODO: X / PO2

      doEvaluateBase(objects, dst, src, ocode);
   }

   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   bool assign;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_div
//
SRCEXP_EXPRBIN_DEFN(div)
{
   return new SourceExpression_BinaryDiv
              (false, exprL, exprR, context, position);
}

//
// SourceExpression::create_binary_div_eq
//
SRCEXP_EXPRBIN_DEFN(div_eq)
{
   return new SourceExpression_BinaryDiv
              (true, exprL, exprR, context, position);
}

//
// SourceExpression_BinaryDiv::SourceExpression_BinaryDiv
//
SourceExpression_BinaryDiv::SourceExpression_BinaryDiv
(bool _assign, SRCEXP_EXPRBIN_PARM)
 : Super(NULL, NULL, SRCEXP_EXPRBIN_PASS), assign(_assign)
{
   CONSTRUCTOR_TYPE_VARS
   CONSTRUCTOR_ARRAY_DECAY

   CONSTRAINT_ARITHMETIC("/")
}

//
// SourceExpression_BinaryDiv::makeObject
//
ObjectExpression::Pointer SourceExpression_BinaryDiv::makeObject() const
{
   EVALUATE_OBJECTS();
   return ObjectExpression::create_binary_div(objL, objR, position);
}

//
// SourceExpression_BinaryDiv::virtual_makeObjects
//
void SourceExpression_BinaryDiv::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   if (assign)
      doAssign(objects, dst);
   else
      doEvaluate(objects, dst);
}

// EOF

