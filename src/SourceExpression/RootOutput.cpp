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
// SourceExpression handling of debugging output.
//
//-----------------------------------------------------------------------------

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../ost_type.hpp"
#include "../SourceException.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_RootOutput
//
class SourceExpression_RootOutput : public SourceExpression
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceExpression_RootOutput,
                                   SourceExpression);

public:
   SourceExpression_RootOutput(SRCEXP_EXPRUNA_ARGS);

private:
   //
   // ::doChar
   //
   void doChar(ObjectVector *objects, char c) const
   {
      objects->addToken(OCODE_GET_IMM, objects->getValue(c));
      objects->addToken(OCODE_ACSP_CHARACTER);
   }

   //
   // ::doChar
   //
   void doChar(ObjectVector *objects, char const *s) const
   {
      while (*s) doChar(objects, *s++);
   }

   void doOut(ObjectVector *objects, VariableType *type) const;

   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst);

   SourceExpression::Pointer expr;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_root_output
//
SourceExpression::Pointer SourceExpression::
create_root_output(SRCEXP_EXPRUNA_ARGS)
{
   return new SourceExpression_RootOutput(expr, context, pos);
}

//
// SourceExpression_RootOutput::SourceExpression_RootOutput
//
SourceExpression_RootOutput::SourceExpression_RootOutput(SRCEXP_EXPRUNA_PARM)
 : Super(SRCEXP_EXPR_PASS), expr(_expr)
{
}

//
// SourceExpression_RootOutput::doOut
//
void SourceExpression_RootOutput::doOut
(ObjectVector *objects, VariableType *type) const
{
   VariableType::BasicType bt = type->getBasicType();
   VariableType::Vector const *types;

   switch (bt)
   {
   case VariableType::BT_VOID:
      doChar(objects, 'V');
      break;

   case VariableType::BT_BIT_HRD:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, 'B');
      break;

   case VariableType::BT_BIT_SFT:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, "BS");
      break;

   case VariableType::BT_CHR:
      doChar(objects, '\'');
      objects->addToken(OCODE_ACSP_CHARACTER);
      doChar(objects, '\'');
      break;

   case VariableType::BT_CLX:
      doChar(objects, "C{");
      doOut(objects, type->getTypes()[1]);
      doChar(objects, ' ');
      doOut(objects, type->getTypes()[0]);
      doChar(objects, '}');
      break;

   case VariableType::BT_CLX_IM:
      doChar(objects, "I{");
      doOut(objects, type->getTypes()[0]);
      doChar(objects, '}');
      break;

   case VariableType::BT_FIX_HH:
      objects->addToken(OCODE_ACSP_NUM_DEC_X);
      doChar(objects, "XHH");
      break;

   case VariableType::BT_FIX_H:
      objects->addToken(OCODE_ACSP_NUM_DEC_X);
      doChar(objects, "XH");
      break;

   case VariableType::BT_FIX:
      objects->addToken(OCODE_ACSP_NUM_DEC_X);
      doChar(objects, 'X');
      break;

   case VariableType::BT_FIX_L:
      objects->addToken(OCODE_ACSP_NUM_DEC_X);
      doChar(objects, "XL");
      break;

   case VariableType::BT_FIX_LL:
      objects->addToken(OCODE_ACSP_NUM_DEC_X);
      doChar(objects, "XLL");
      break;

   case VariableType::BT_FLT_HH:
   case VariableType::BT_FLT_H:
   case VariableType::BT_FLT:
   case VariableType::BT_FLT_L:
   case VariableType::BT_FLT_LL:
      ERROR_NP("unsupported BT: %s", make_string(bt).c_str());

   case VariableType::BT_INT_HH:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, "IHH");
      break;

   case VariableType::BT_INT_H:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, "IH");
      break;

   case VariableType::BT_INT:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, 'I');
      break;

   case VariableType::BT_INT_L:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, "IL");
      break;

   case VariableType::BT_INT_LL:
      objects->addToken(OCODE_ACSP_NUM_HEX_U);
      doChar(objects, ' ');
      objects->addToken(OCODE_ACSP_NUM_HEX_U);
      doChar(objects, "ILL");
      break;

   case VariableType::BT_UNS_HH:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, "UHH");
      break;

   case VariableType::BT_UNS_H:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, "UH");
      break;

   case VariableType::BT_UNS:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, 'U');
      break;

   case VariableType::BT_UNS_L:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, "UL");
      break;

   case VariableType::BT_UNS_LL:
      objects->addToken(OCODE_ACSP_NUM_HEX_U);
      doChar(objects, ' ');
      objects->addToken(OCODE_ACSP_NUM_HEX_U);
      doChar(objects, "ULL");
      break;

   case VariableType::BT_LABEL:
      objects->addToken(OCODE_ACSP_NUM_HEX_U);
      doChar(objects, 'L');
      break;

   case VariableType::BT_STR:
      doChar(objects, '"');
      objects->addToken(OCODE_ACSP_STR);
      doChar(objects, '"');
      break;

   case VariableType::BT_ARR:
      doChar(objects, "A{");
      for (bigsint i = type->getWidth(); i--;)
      {
         doOut(objects, type->getReturn());
         if (i) doChar(objects, ' ');
      }
      doChar(objects, "}A");
      break;

   case VariableType::BT_PTR:
      if(VariableType::is_bt_function(type->getReturn()->getBasicType()))
      {
         doOut(objects, type->getReturn());
      }
      else for(biguint i = type->getSize(pos); i--;)
      {
         objects->addToken(OCODE_ACSP_NUM_HEX_U);
         if(i) doChar(objects, ' ');
      }
      doChar(objects, 'P');
      break;

   case VariableType::BT_PTR_NUL:
      objects->addToken(OCODE_ACSP_NUM_HEX_U);
      doChar(objects, "PN");
      break;

   case VariableType::BT_ENUM:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, 'E');
      break;

   case VariableType::BT_STRUCT:
      doChar(objects, "S{");
      for (size_t i = (types = &type->getTypes())->size(); i--;)
      {
         doOut(objects, (*types)[i]);
         if (i) doChar(objects, ' ');
      }
      doChar(objects, "}S");
      break;

   case VariableType::BT_UNION:
      doChar(objects, "U{");
      for (bigsint i = type->getSize(pos); i--;)
      {
         objects->addToken(OCODE_ACSP_NUM_HEX_U);
         if (i) doChar(objects, ' ');
      }
      doChar(objects, "}U");

      break;

   case VariableType::BT_BLOCK:
      doChar(objects, "B{");
      for (size_t i = (types = &type->getTypes())->size(); i--;)
      {
         doOut(objects, (*types)[i]);
         if (i) doChar(objects, ' ');
      }
      doChar(objects, "}B");
      break;

   case VariableType::BT_FUN:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, 'f');
      break;

   case VariableType::BT_FUN_ASM:
      doChar(objects, "fA");
      break;

   case VariableType::BT_FUN_LIN:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, "fL");
      break;

   case VariableType::BT_FUN_NAT:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, "fN");
      break;

   case VariableType::BT_FUN_SNA:
      doChar(objects, '"');
      objects->addToken(OCODE_ACSP_STR);
      doChar(objects, "\"fSa");
      break;

   case VariableType::BT_FUN_SNU:
      objects->addToken(OCODE_ACSP_NUM_DEC_I);
      doChar(objects, "fSu");
      break;
   }

   doChar(objects, ';');
}

//
// SourceExpression_RootOutput::virtual_makeObjects
//
void SourceExpression_RootOutput::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   expr->makeObjects
      (objects, VariableData::create_stack(expr->getType()->getSize(pos)));

   objects->setPosition(pos);

   objects->addToken(OCODE_ACSP_START);

   doOut(objects, expr->getType());

   if(Target == TARGET_Hexen)
      objects->addToken(OCODE_ACSP_END);
   else
      objects->addToken(OCODE_ACSP_END_LOG);
}

// EOF

