//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2014 David Hill
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
// SourceExpression handling of "operator >>" and "operator >>=".
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
// SourceExpression_BinaryRSh
//
class SourceExpression_BinaryRSh : public SourceExpression_Binary
{
   CounterPreambleNoClone(SourceExpression_BinaryRSh, SourceExpression_Binary);

public:
   //
   // ::SourceExpression_BinaryRSh
   //
   SourceExpression_BinaryRSh(SRCEXP_EXPRBIN_PARM, bool _assign)
    : Super(SRCEXP_EXPRBIN_PASS, NULL, NULL, _assign)
   {
      CONSTRUCTOR_TYPE_VARS();
      CONSTRUCTOR_ARRAY_DECAY();

      CONSTRAINT_INTEGER(">>");

      docast = false;
      exprR = create_value_cast_implicit(exprR, VariableType::get_bt_int(), context, pos);
   }

   //
   // ::canMakeObject
   //
   virtual bool canMakeObject() const
   {
      return false;
   }

   //
   // ::getType
   //
   VariableType::Reference getType() const
   {
      return exprL->getType();
   }

protected:
   //
   // ::canDoSet
   //
   virtual bool canDoSet(VariableData *data, VariableType *type) const
   {
      if(VariableType::IsTypeUnsigned(type->getBasicType()))
         return false;

      CAN_SET_SWITCHES(RSH);
   }

   //
   // ::doGet
   //
   virtual void doGet(ObjectVector *objects, VariableType *type, int)
   {
      switch(type->getBasicType())
      {
         DO_GET_CASES(RSH);

      case VariableType::BT_FIX_L:
      case VariableType::BT_FIX_LL:
      case VariableType::BT_INT_L:
      case VariableType::BT_INT_LL:
         objects->addToken(OCODE_JMP_CAL_IMM, objects->getValue("__IrshL"));
         objects->addToken(OCODE_GET_IMM,     objects->getValue(-1));
         objects->addToken(OCODE_GET_WLDARR,  objects->getValue(option_auto_array));
         break;

      case VariableType::BT_UNS_L:
      case VariableType::BT_UNS_LL:
         objects->addToken(OCODE_JMP_CAL_IMM, objects->getValue("__UrshL"));
         objects->addToken(OCODE_GET_IMM,     objects->getValue(-1));
         objects->addToken(OCODE_GET_WLDARR,  objects->getValue(option_auto_array));
         break;
      }
   }

   //
   // ::doSet
   //
   virtual void doSet(ObjectVector *objects, VariableData *data, VariableType *type, int)
   {
      DO_SET_SWITCHES(RSH);
   }
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_rsh
//
SRCEXP_EXPRBIN_DEFN(rsh)
{
   return new SourceExpression_BinaryRSh(exprL, exprR, context, pos, false);
}

//
// SourceExpression::create_binary_rsh_eq
//
SRCEXP_EXPRBIN_DEFN(rsh_eq)
{
   return new SourceExpression_BinaryRSh(exprL, exprR, context, pos, true);
}

// EOF

