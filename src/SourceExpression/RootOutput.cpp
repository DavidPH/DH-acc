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
   return new SourceExpression_RootOutput(expr, context, position);
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
   VariableType::Vector const *types;

   switch (type->getBasicType())
   {
   case VariableType::BT_ARRAY:
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('A'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('{'));
      objects->addToken(OCODE_ACSP_CHARACTER);

      for (bigsint i = type->getWidth(); i--;)
      {
         doOut(objects, type->getReturn());

         if (i)
         {
            objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(' '));
            objects->addToken(OCODE_ACSP_CHARACTER);
         }
      }

      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('}'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('A'));
      objects->addToken(OCODE_ACSP_CHARACTER);

      break;

   case VariableType::BT_ASMFUNC:
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('P'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('A'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_BLOCK:
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('B'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('{'));
      objects->addToken(OCODE_ACSP_CHARACTER);

      types = &type->getTypes();
      for (size_t i = types->size(); i--;)
      {
         doOut(objects, (*types)[i]);

         if (i)
         {
            objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(' '));
            objects->addToken(OCODE_ACSP_CHARACTER);
         }
      }

      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('}'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('B'));
      objects->addToken(OCODE_ACSP_CHARACTER);

      break;

   case VariableType::BT_BOOLHARD:
      objects->addToken(OCODE_ACSP_NUM_DEC32I);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('B'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_BOOLSOFT:
      objects->addToken(OCODE_ACSP_NUM_DEC32I);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('B'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('S'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_CHAR:
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('\''));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('\''));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_ENUM:
      objects->addToken(OCODE_ACSP_NUM_DEC32I);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('E'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_FIXED:
      objects->addToken(OCODE_ACSP_NUM_DEC32F);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('F'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('I'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_FUNCTION:
      objects->addToken(OCODE_ACSP_NUM_DEC32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('P'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('F'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_INT:
      objects->addToken(OCODE_ACSP_NUM_DEC32I);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('I'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_LABEL:
      objects->addToken(OCODE_ACSP_NUM_HEX32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('L'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_LINESPEC:
      objects->addToken(OCODE_ACSP_NUM_DEC32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('P'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('L'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_LLONG:
      objects->addToken(OCODE_ACSP_NUM_HEX32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(' '));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_ACSP_NUM_HEX32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('L'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('L'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_LONG:
      objects->addToken(OCODE_ACSP_NUM_DEC32I);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('L'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_NATIVE:
      objects->addToken(OCODE_ACSP_NUM_DEC32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('P'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('N'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_POINTER:
      objects->addToken(OCODE_ACSP_NUM_HEX32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('P'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_REAL:
      objects->addToken(OCODE_ACSP_NUM_DEC32F);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('F'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_SCHAR:
      objects->addToken(OCODE_ACSP_NUM_DEC32I);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('H'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('H'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_SCRIPT:
      objects->addToken(OCODE_ACSP_NUM_DEC32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('P'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('S'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_SHORT:
      objects->addToken(OCODE_ACSP_NUM_DEC32I);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('H'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_STRING:
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('"'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_ACSP_STRING);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('"'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_STRUCT:
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('S'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('{'));
      objects->addToken(OCODE_ACSP_CHARACTER);

      types = &type->getTypes();
      for (size_t i = types->size(); i--;)
      {
         doOut(objects, (*types)[i]);

         if (i)
         {
            objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(' '));
            objects->addToken(OCODE_ACSP_CHARACTER);
         }
      }

      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('}'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('S'));
      objects->addToken(OCODE_ACSP_CHARACTER);

      break;

   case VariableType::BT_UCHAR:
      objects->addToken(OCODE_ACSP_NUM_DEC32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('U'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('H'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('H'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_UINT:
      objects->addToken(OCODE_ACSP_NUM_DEC32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('U'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_ULLONG:
      objects->addToken(OCODE_ACSP_NUM_HEX32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(' '));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_ACSP_NUM_HEX32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('U'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('L'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('L'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_ULONG:
      objects->addToken(OCODE_ACSP_NUM_DEC32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('U'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('L'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_UNION:
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('U'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('{'));
      objects->addToken(OCODE_ACSP_CHARACTER);

      for (bigsint i = type->getSize(position); i--;)
      {
         objects->addToken(OCODE_ACSP_NUM_HEX32U);

         if (i)
         {
            objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(' '));
            objects->addToken(OCODE_ACSP_CHARACTER);
         }
      }

      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('}'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('U'));
      objects->addToken(OCODE_ACSP_CHARACTER);

      break;

   case VariableType::BT_USHORT:
      objects->addToken(OCODE_ACSP_NUM_DEC32U);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('U'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('H'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_VOID:
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue('V'));
      objects->addToken(OCODE_ACSP_CHARACTER);
      break;

   case VariableType::BT_FLOAT:
   case VariableType::BT_LFLOAT:
   case VariableType::BT_LLFLOAT:
      throw SourceException("unsupported BT", position, getName());
   }

   objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(';'));
   objects->addToken(OCODE_ACSP_CHARACTER);
}

//
// SourceExpression_RootOutput::virtual_makeObjects
//
void SourceExpression_RootOutput::virtual_makeObjects
(ObjectVector *objects, VariableData *dst)
{
   Super::recurse_makeObjects(objects, dst);

   expr->makeObjects
   (objects, VariableData::create_stack(expr->getType()->getSize(position)));

   objects->setPosition(position);

   objects->addToken(OCODE_ACSP_START);

   doOut(objects, expr->getType());

   if (target_type == TARGET_Hexen)
      objects->addToken(OCODE_ACSP_END);
   else
      objects->addToken(OCODE_ACSP_END_LOG);
}

// EOF

