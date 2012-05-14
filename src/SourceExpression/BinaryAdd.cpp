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
   //
   // ::SourceExpression_BinaryAdd
   //
   SourceExpression_BinaryAdd(bool _assign, SRCEXP_EXPRBIN_PARM)
    : Super(SRCEXP_EXPRBIN_PASS, NULL, NULL, _assign)
   {
      CONSTRUCTOR_TYPE_VARS();
      CONSTRUCTOR_ARRAY_DECAY();

      // Type constraints.
      if (btL == VariableType::BT_PTR || btR == VariableType::BT_PTR)
      {
         // Pointer constraints.
         if (btL == VariableType::BT_PTR && btR == VariableType::BT_PTR)
            ERROR_NP("pointer + pointer");

         if (btL == VariableType::BT_PTR && !VariableType::is_bt_integer(btR))
            ERROR_NP("pointer + non-integer");

         if (!VariableType::is_bt_integer(btL) && btR == VariableType::BT_PTR)
            ERROR_NP("non-integer + pointer");
      }
      else
      {
         CONSTRAINT_ARITHMETIC("+");
      }

      if (assign && !VariableType::is_bt_arithmetic(btR))
         ERROR_NP("X -= non-arithmetic");

      CONSTRUCTOR_POINTER_PREAMBLE();

      if (btL == VariableType::BT_PTR)
         exprR = create_binary_mul(exprR, exprSize, context, pos);

      if (btR == VariableType::BT_PTR)
         exprL = create_binary_mul(exprL, exprSize, context, pos);
   }

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      EVALUATE_OBJECTS();
      return ObjectExpression::create_binary_add(objL, objR, pos);
   }

protected:
   //
   // ::doGet
   //
   virtual void doGet(ObjectVector *objects, VariableType *type, int tmpBase)
   {
      switch (type->getBasicType())
      {
         DO_GET_CASES(ADD);

      case VariableType::BT_INT_LL: doGetBaseILLAS(objects, type, tmpBase, true); break;
      case VariableType::BT_UNS_LL: doGetBaseILLAS(objects, type, tmpBase, true); break;
      }
   }

   //
   // ::doSet
   //
   virtual bool doSet(ObjectVector *objects, VariableData *data,
                      VariableType *type, int)
   {
      DO_SET_SWITCHES(ADD);
      return false;
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_add
//
SRCEXP_EXPRBIN_DEFN(add)
{
   return new SourceExpression_BinaryAdd(false, exprL, exprR, context, pos);
}

//
// SourceExpression::create_binary_add_eq
//
SRCEXP_EXPRBIN_DEFN(add_eq)
{
   return new SourceExpression_BinaryAdd(true, exprL, exprR, context, pos);
}

// EOF

