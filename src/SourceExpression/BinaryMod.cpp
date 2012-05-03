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
// SourceExpression handling of "operator %" and "operator %=".
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
// SourceExpression_BinaryMod
//
class SourceExpression_BinaryMod : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryMod,
                                   SourceExpression_Binary);

public:
   SourceExpression_BinaryMod(bool assign, SRCEXP_EXPRBIN_ARGS);

   virtual CounterPointer<ObjectExpression> makeObject() const;

private:
   //
   // ::doAssign
   //
   void doAssign(ObjectVector *objects, VariableData *dst)
   {
      ASSIGN_ARITHMETIC_VARS

      ASSIGN_GET_OCODE_ARITHMETIC(MOD)

      doAssignBase(objects, dst, src, ocodeOp, ocodeGet);
   }

   //
   // ::doEvaluate
   //
   void doEvaluate(ObjectVector *objects, VariableData *dst)
   {
      EVALUATE_ARITHMETIC_VARS(MOD)

      ocode = static_cast<ObjectCode>(ocode + getOcodeType(bt));

      // TODO: X % PO2

      doEvaluateBase(objects, dst, src, ocode);
   }

   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   bool assign;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_mod
//
SRCEXP_EXPRBIN_DEFN(mod)
{
   return new SourceExpression_BinaryMod(false, exprL, exprR, context, pos);
}

//
// SourceExpression::create_binary_mod_eq
//
SRCEXP_EXPRBIN_DEFN(mod_eq)
{
   return new SourceExpression_BinaryMod(true, exprL, exprR, context, pos);
}

//
// SourceExpression_BinaryMod::SourceExpression_BinaryMod
//
SourceExpression_BinaryMod::SourceExpression_BinaryMod
(bool _assign, SRCEXP_EXPRBIN_PARM)
 : Super(NULL, NULL, SRCEXP_EXPRBIN_PASS), assign(_assign)
{
   CONSTRUCTOR_TYPE_VARS
   CONSTRUCTOR_ARRAY_DECAY

   CONSTRAINT_ARITHMETIC("%%")
}

//
// SourceExpression_BinaryMod::makeObject
//
ObjectExpression::Pointer SourceExpression_BinaryMod::makeObject() const
{
   EVALUATE_OBJECTS();
   return ObjectExpression::create_binary_mod(objL, objR, pos);
}

//
// SourceExpression_BinaryMod::virtual_makeObjects
//
void SourceExpression_BinaryMod::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   if (assign)
      doAssign(objects, dst);
   else
      doEvaluate(objects, dst);
}

// EOF

