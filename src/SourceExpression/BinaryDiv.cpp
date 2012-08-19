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
   //
   // ::SourceExpression_BinaryDiv
   //
   SourceExpression_BinaryDiv(SRCEXP_EXPRBIN_PARM, bool _assign)
    : Super(SRCEXP_EXPRBIN_PASS, NULL, NULL, _assign)
   {
      CONSTRUCTOR_TYPE_VARS();
      CONSTRUCTOR_ARRAY_DECAY();

      CONSTRAINT_ARITHMETIC("/");
   }

   //
   // ::makeObject
   //
   virtual ObjectExpression::Pointer makeObject() const
   {
      EVALUATE_OBJECTS();
      return ObjectExpression::create_binary_div(objL, objR, pos);
   }

protected:
   //
   // ::canDoSet
   //
   virtual bool canDoSet(VariableData *data, VariableType *type) const
   {
      if(VariableType::is_bt_fix(type->getBasicType())) return false;

      CAN_SET_SWITCHES(DIV);
   }

   //
   // ::doGet
   //
   virtual void doGet(ObjectVector *objects, VariableType *type, int)
   {
      switch(type->getBasicType())
      {
         DO_GET_CASES(DIV);

      case VariableType::BT_INT_L:
      case VariableType::BT_INT_LL:
         objects->addToken(OCODE_JMP_CAL_IMM, objects->getValue("_ldiv"));

         objects->addToken(OCODE_GET_IMM,    objects->getValue(-3));
         objects->addToken(OCODE_GET_WLDARR, objects->getValue(option_auto_array));
         break;

      case VariableType::BT_UNS_L:
      case VariableType::BT_UNS_LL:
         objects->addToken(OCODE_JMP_CAL_IMM, objects->getValue("__UdivL"));

         objects->addToken(OCODE_GET_IMM,    objects->getValue(-3));
         objects->addToken(OCODE_GET_WLDARR, objects->getValue(option_auto_array));
         break;
      }
   }

   //
   // ::doSet
   //
   virtual void doSet(ObjectVector *objects, VariableData *data, VariableType *type, int)
   {
      DO_SET_SWITCHES(DIV);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_div
//
SRCEXP_EXPRBIN_DEFN(div)
{
   return new SourceExpression_BinaryDiv(exprL, exprR, context, pos, false);
}

//
// SourceExpression::create_binary_div_eq
//
SRCEXP_EXPRBIN_DEFN(div_eq)
{
   return new SourceExpression_BinaryDiv(exprL, exprR, context, pos, true);
}

// EOF

