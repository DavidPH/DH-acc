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
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_BinaryRSh,
                                   SourceExpression_Binary);

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

      exprR = create_value_cast_implicit
         (exprR, VariableType::get_bt_uns(), context, pos);
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
   // ::doGet
   //
   virtual void doGet(ObjectVector *objects, VariableType *type, int tmpBase)
   {
      if (VariableType::is_bt_unsigned(type->getBasicType()))
         return doGetU(objects, type, tmpBase);

      DO_GET_SWITCH(RSH);
   }

   //
   // ::doSet
   //
   virtual bool doSet(ObjectVector *objects, VariableData *data,
                      VariableType *type, int)
   {
      if (VariableType::is_bt_unsigned(type->getBasicType()))
         return false;

      DO_SET_SWITCHES(RSH);
      return false;
   }

private:
   //
   // ::doGetU
   //
   void doGetU(ObjectVector *objects, VariableType *, int tmpBase)
   {
      ObjectExpression::Pointer tmpS = context->getTempVar(tmpBase+0);

      std::string label = context->makeLabel();
      std::string labelEnd = label + "_end";

      objects->addToken(OCODE_SET_TEMP, tmpS);
      objects->addToken(OCODE_GET_TEMP, tmpS);
      objects->addToken(OCODE_RSH_STK_I);
      objects->addToken(OCODE_GET_TEMP, tmpS);
      objects->addToken(OCODE_JMP_NIL, objects->getValue(labelEnd));
      objects->addToken(OCODE_GET_IMM, objects->getValue(0x80000000));
      objects->addToken(OCODE_DEC_TEMP_U, tmpS);
      objects->addToken(OCODE_GET_TEMP, tmpS);
      objects->addToken(OCODE_RSH_STK_I);
      objects->addToken(OCODE_INV_STK_U);
      objects->addToken(OCODE_AND_STK_U);

      objects->addLabel(labelEnd);
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

