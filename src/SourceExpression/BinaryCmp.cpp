//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 David Hill
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
// SourceExpression handling of relational operators.
//
//-----------------------------------------------------------------------------

#include "Binary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../SourceContext.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// SourceExpression_BinaryCmp
//
class SourceExpression_BinaryCmp : public SourceExpression_Binary
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE
   (SourceExpression_BinaryCmp, SourceExpression_Binary);

public:
   enum CmpType
   {
      CMP_GE,
      CMP_GT,
      CMP_LE,
      CMP_LT,
      CMP_EQ,
      CMP_NE
   };

   //
   // ::SourceExpression_BinaryCmp
   //
   SourceExpression_BinaryCmp(CmpType _ct, SRCEXP_EXPRBIN_PARM)
    : Super(false, SRCEXP_EXPRBIN_PASS), ct(_ct)
   {
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
   virtual VariableType::Reference getType() const
   {
      return VariableType::get_bt_boolhard();
   }

   //
   // ::virtual_makeObjects
   //
   virtual void virtual_makeObjects(ObjectVector *objects, VariableData *dst)
   {
      Super::recurse_makeObjects(objects, dst);

      VariableType::Reference inType = Super::getType();
      bigsint                 inSize = inType->getSize(position);
      VariableType::BasicType inBT = inType->getBasicType();
      VariableType::Reference type = getType();
      bigsint                 size = type->getSize(position);
      VariableData::Pointer   src  = VariableData::create_stack(size);

      int ocodeType;

      getOcodeType(inBT, &ocodeType);

      ObjectCode ocode = OCODE_NONE;
      switch (ct)
      {
      case CMP_GE: ocode = OCODE_CMP_GE32F; break;
      case CMP_GT: ocode = OCODE_CMP_GT32F; break;
      case CMP_LE: ocode = OCODE_CMP_LE32F; break;
      case CMP_LT: ocode = OCODE_CMP_LT32F; break;
      case CMP_EQ: ocode = OCODE_CMP_EQ32F; break;
      case CMP_NE: ocode = OCODE_CMP_NE32F; break;
      }

      ocode = static_cast<ObjectCode>(ocode + ocodeType);

      if (ct < CMP_EQ && VariableType::is_bt_unsigned(inBT) && inSize == 1)
         return makeObjectsU(objects, ocode, dst, src, type);

      objects->addToken(ocode);

      make_objects_memcpy_post(objects, dst, src, type, position);
   }

private:
   //
   // ::makeObjectsU
   //
   void makeObjectsU
   (ObjectVector *objects, ObjectCode ocode, VariableData *dst,
    VariableData *src, VariableType *type)
   {
      std::string label = context->makeLabel();
      std::string labelCmp = label + "_cmp";
      std::string labelEnd = label + "_end";
      std::string labelPos = label + "_pos";

      ObjectExpression::Pointer tmpJ = context->getTempVar(0);
      ObjectExpression::Pointer tmpK = context->getTempVar(1);

      // unsigned j; unsigned k;
      objects->addToken(OCODE_SET_REGISTER32I, tmpK);
      objects->addToken(OCODE_SET_REGISTER32I, tmpJ);

      // if (j & 0x80000000) ...
      objects->addToken(OCODE_GET_REGISTER32I, tmpJ);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0x80000000));
      objects->addToken(OCODE_BITWISE_AND32);
      objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(labelPos));

      // ... then
      //    if (!(k & 0x80000000)) return +1;
      objects->addToken(OCODE_GET_REGISTER32I, tmpK);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0x80000000));
      objects->addToken(OCODE_BITWISE_AND32);
      objects->addToken(OCODE_BRANCH_TRUE, objects->getValue(labelCmp));
      if (ct == CMP_GE || ct == CMP_GT)
         objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(1));
      else
         objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0));
      objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(labelEnd));

      // ... else
      objects->addLabel(labelPos);
      //    if (k & 0x80000000) return -1;
      objects->addToken(OCODE_GET_REGISTER32I, tmpK);
      objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0x80000000));
      objects->addToken(OCODE_BITWISE_AND32);
      objects->addToken(OCODE_BRANCH_ZERO, objects->getValue(labelCmp));
      if (ct == CMP_LE || ct == CMP_LT)
         objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(1));
      else
         objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(0));
      objects->addToken(OCODE_BRANCH_GOTO_IMM, objects->getValue(labelEnd));

      // if (j < k) return -1;
      // if (j > k) return +1;
      objects->addLabel(labelCmp);
      objects->addToken(OCODE_GET_REGISTER32I, tmpJ);
      objects->addToken(OCODE_GET_REGISTER32I, tmpK);
      objects->addToken(ocode);

      objects->addLabel(labelEnd);

      make_objects_memcpy_post(objects, dst, src, type, position);
   }

   CmpType ct;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpression::create_binary_cmp_ge
//
SRCEXP_EXPRBIN_DEFN(cmp_ge)
{
   return new SourceExpression_BinaryCmp
   (SourceExpression_BinaryCmp::CMP_GE, exprL, exprR, context, position);
}

//
// SourceExpression::create_binary_cmp_gt
//
SRCEXP_EXPRBIN_DEFN(cmp_gt)
{
   return new SourceExpression_BinaryCmp
   (SourceExpression_BinaryCmp::CMP_GT, exprL, exprR, context, position);
}

//
// SourceExpression::create_binary_cmp_le
//
SRCEXP_EXPRBIN_DEFN(cmp_le)
{
   return new SourceExpression_BinaryCmp
   (SourceExpression_BinaryCmp::CMP_LE, exprL, exprR, context, position);
}

//
// SourceExpression::create_binary_cmp_lt
//
SRCEXP_EXPRBIN_DEFN(cmp_lt)
{
   return new SourceExpression_BinaryCmp
   (SourceExpression_BinaryCmp::CMP_LT, exprL, exprR, context, position);
}

//
// SourceExpression::create_binary_cmp_eq
//
SRCEXP_EXPRBIN_DEFN(cmp_eq)
{
   return new SourceExpression_BinaryCmp
   (SourceExpression_BinaryCmp::CMP_EQ, exprL, exprR, context, position);
}

//
// SourceExpression::create_binary_cmp_ne
//
SRCEXP_EXPRBIN_DEFN(cmp_ne)
{
   return new SourceExpression_BinaryCmp
   (SourceExpression_BinaryCmp::CMP_NE, exprL, exprR, context, position);
}

// EOF

